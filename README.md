# STM32G474 FOC 电机控制

基于 STM32G474 的高性能磁场定向控制（FOC）实现，从环境搭建到三环控制，逐步深入。项目采用 STM32CubeMX 生成外设初始化代码，CLion/VSCode 作为开发环境，使用 CMSIS-DSP 加速数学运算，适合学习 FOC 原理或作为电机控制项目的开发起点。

## 硬件平台

| 组件 | 型号/参数 |
|------|-----------|
| 核心板 | STM32G474RET6（170MHz，Cortex-M4F，硬件 FPU） |
| 驱动板 | X-NUCLEO-IHM07M1 或 BOOSTXL-DRV8301 |
| 电机 | 2804 云台电机（表贴式 PMSM，12V，1A） |
| 电流采样 | 双电阻采样（330mΩ），或单电阻/三电阻 |
| 位置反馈 | 霍尔传感器 / AS5600 磁编码器 |

STM32G474 的 HRTIM（高分辨率定时器）可产生 0.68GHz 等效时钟的 PWM，内置 CORDIC 硬件加速三角函数计算，5MSPS ADC 满足 20kHz 电流环的实时性要求。

## 环境搭建

### 工具链

| 工具 | 说明 |
|------|------|
| STM32CubeMX | 外设配置与代码生成 |
| CLion / VSCode | 代码编辑与调试 |
| ARM GNU Toolchain | `arm-none-eabi-gcc` 编译链 |
| OpenOCD | DAPLink/ST-Link 烧录调试 |
| VOFA+ | 上位机波形显示（JustFloat 协议） |

### 快速开始

```bash
# 克隆仓库
git clone https://github.com/your-username/stm32g474-foc.git
cd stm32g474-foc

# 生成构建系统（CLion 自动识别 CMakeLists.txt）
# 或手动编译
cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/arm-gcc-toolchain.cmake
cmake --build build

# 烧录
openocd -f interface/cmsis-dap.cfg -f target/stm32g4x.cfg -c "program build/stm32g474-foc.elf verify reset exit"
```

### 关键外设配置

**PWM 生成（HRTIM 或 TIM1）**

采用中心对齐模式产生三相互补 PWM。以 TIM1 为例，PWM 频率 20kHz：

```c
// 预分频 0，ARR = 170e6 / (20e3 * 2) - 1 = 4249
htim1.Init.Prescaler = 0;
htim1.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED1;
htim1.Init.Period = 4249;
```

使用 HRTIM 时，Master Timer 频率 0.68GHz，period 值 34000 对应 20kHz 中心对称 PWM。

**ADC 触发**

将 PWM 定时器的更新事件或通道 4 比较匹配作为 ADC 触发源，确保在电流纹波稳定时刻采样。双电阻采样时，ADC 注入通道序列配置为两相电流，第三相由 `ic = -ia - ib` 计算得出。

**串口通信**

USART1 配置 115200 波特率，用于 VOFA+ 波形显示和命令行调试。数据帧格式遵循 JustFloat 协议：`[float data0] [float data1] ... [0x00 0x00 0x80 0x7f]`。

## 控制架构演进

### 第一阶段：开环控制

开环控制不依赖位置反馈，直接给定电压矢量的幅值和电角度频率旋转电机。用于验证 PWM 输出、驱动板接线和电流采样是否正常。

```c
typedef struct {
    float Vd;           // d 轴电压给定
    float Vq;           // q 轴电压给定
    float theta_e;      // 电角度
    float omega_e;      // 电角速度 (rad/s)
} OpenLoop_Handle_t;

void OpenLoop_Run(OpenLoop_Handle_t *ol, float dt) {
    ol->theta_e += ol->omega_e * dt;
    // 归一化到 [0, 2π)
    if (ol->theta_e > 2.0f * M_PI) ol->theta_e -= 2.0f * M_PI;
    
    // 逆 Park 变换
    float Valpha = ol->Vd * cosf(ol->theta_e) - ol->Vq * sinf(ol->theta_e);
    float Vbeta  = ol->Vd * sinf(ol->theta_e) + ol->Vq * cosf(ol->theta_e);
    
    // SVPWM 输出
    SVPWM_Output(Valpha, Vbeta, Vbus);
}
```

开环调试要点：先给较小的 `Vq`（如 0.5V），缓慢增加 `omega_e`，观察电机是否平稳旋转。若电机抖动或堵转，检查相序、死区配置或驱动使能引脚。

### 第二阶段：电流环

电流环是 FOC 的核心。通过 Clarke/Park 变换将三相电流映射到 d-q 旋转坐标系，对 `Id`（励磁分量）和 `Iq`（转矩分量）分别进行 PI 调节。

```c
typedef struct {
    float Kp, Ki;
    float integral;
    float out_max, out_min;
    float ref, fdb;
} PI_Handle_t;

float PI_Calc(PI_Handle_t *pi, float ref, float fdb, float dt) {
    pi->ref = ref;
    pi->fdb = fdb;
    float err = ref - fdb;
    
    pi->integral += pi->Ki * err * dt;
    // 抗积分饱和
    if (pi->integral > pi->out_max) pi->integral = pi->out_max;
    if (pi->integral < pi->out_min) pi->integral = pi->out_min;
    
    float out = pi->Kp * err + pi->integral;
    // 输出限幅
    if (out > pi->out_max) out = pi->out_max;
    if (out < pi->out_min) out = pi->out_min;
    return out;
}
```

电流环执行频率应与 PWM 频率一致（通常 20kHz），在 ADC 转换完成中断中运行。`Id_ref` 设为 0（表贴式电机 MTPA 策略），`Iq_ref` 来自速度环输出或直接给定。

```c
// 电流环中断服务程序（简化）
void ADC_IRQHandler(void) {
    // 1. 读取电流
    float ia = ADC_GetCurrentA();
    float ib = ADC_GetCurrentB();
    float ic = -ia - ib;
    
    // 2. Clarke 变换
    float Ialpha = ia;
    float Ibeta  = (ia + 2.0f * ib) * ONE_BY_SQRT3;
    
    // 3. Park 变换
    float theta = Encoder_GetElecAngle();
    float Id =  Ialpha * cosf(theta) + Ibeta * sinf(theta);
    float Iq = -Ialpha * sinf(theta) + Ibeta * cosf(theta);
    
    // 4. PI 调节
    float Vd = PI_Calc(&pid_id, 0.0f, Id, DT);
    float Vq = PI_Calc(&pid_iq, iq_ref, Iq, DT);
    
    // 5. 逆 Park + SVPWM
    float Valpha = Vd * cosf(theta) - Vq * sinf(theta);
    float Vbeta  = Vd * sinf(theta) + Vq * cosf(theta);
    SVPWM_Output(Valpha, Vbeta, Vbus);
}
```

电流环 PI 参数整定建议从较小 `Kp` 开始，逐步增加直至电流响应快速且无明显超调。`Ki` 用于消除稳态误差。

### 第三阶段：速度环 + 电流环

速度环作为外环，以较低频率（通常 1kHz）运行，输出作为电流环的 `Iq_ref`。

```c
typedef struct {
    PI_Handle_t pi;
    float target_rpm;
    float actual_rpm;
    float iq_out;
} SpeedLoop_Handle_t;

void SpeedLoop_Run(SpeedLoop_Handle_t *sl, float dt) {
    // 计算转速（来自编码器差分或霍尔频率测量）
    sl->actual_rpm = Encoder_GetSpeedRPM();
    
    // PI 调节，输出 Iq 参考值
    sl->iq_out = PI_Calc(&sl->pi, sl->target_rpm, sl->actual_rpm, dt);
    
    // 限幅（保护电机和驱动器）
    if (sl->iq_out > CURRENT_MAX) sl->iq_out = CURRENT_MAX;
    if (sl->iq_out < -CURRENT_MAX) sl->iq_out = -CURRENT_MAX;
}

// 在定时器中断中以 1kHz 调用
void SpeedLoop_TIM_IRQHandler(void) {
    SpeedLoop_Run(&speed_loop, 1.0f / 1000.0f);
    iq_ref = speed_loop.iq_out;  // 传递给电流环
}
```

速度环带宽应远低于电流环（通常 1:10 到 1:20），以避免环路耦合振荡。

### 第四阶段：三环控制（位置 + 速度 + 电流）

位置环作为最外层，以 1kHz 运行，输出速度指令给速度环。

```c
typedef struct {
    PI_Handle_t pi;
    float target_deg;
    float actual_deg;
    float speed_out;
    float speed_limit;   // 最大速度限幅
} PositionLoop_Handle_t;

void PositionLoop_Run(PositionLoop_Handle_t *pl, float dt) {
    pl->actual_deg = Encoder_GetMechanicalAngleDeg();
    
    // 处理角度环绕（0~360 度）
    float err = pl->target_deg - pl->actual_deg;
    if (err > 180.0f)  err -= 360.0f;
    if (err < -180.0f) err += 360.0f;
    
    pl->speed_out = PI_Calc(&pl->pi, 0.0f, -err, dt);  // 误差取负，使 PI 输出方向正确
    
    // 速度限幅
    if (pl->speed_out > pl->speed_limit)  pl->speed_out = pl->speed_limit;
    if (pl->speed_out < -pl->speed_limit) pl->speed_out = -pl->speed_limit;
}
```

三环级联结构如下，参数整定顺序为：**先电流环 → 再速度环 → 最后位置环**。

```
位置环 (1kHz) ──► 速度环 (1kHz) ──► 电流环 (20kHz) ──► SVPWM ──► 电机
     ▲                  ▲                  ▲
     │                  │                  │
   编码器角度          编码器速度         相电流
```

## 项目结构

```
stm32g474-foc/
├── Core/
│   ├── Src/
│   │   ├── main.c              # 主循环与初始化
│   │   ├── foc.c               # FOC 核心算法
│   │   ├── pid.c               # PI 调节器
│   │   ├── svpwm.c             # SVPWM 调制
│   │   └── encoder.c           # 编码器/霍尔接口
│   └── Inc/
├── Drivers/
│   ├── STM32G4xx_HAL_Driver/
│   └── CMSIS/
├── cmake/
│   └── arm-gcc-toolchain.cmake
├── STM32G474RETx_FLASH.ld
└── CMakeLists.txt
```

## 调试与可视化

通过 USART 将关键变量发送至 VOFA+，实时观察电流、速度和位置波形。

```c
// 在控制循环中周期性调用（如 1kHz）
void Telemetry_Send(void) {
    float data[4];
    data[0] = motor.Id;
    data[1] = motor.Iq;
    data[2] = speed_loop.actual_rpm;
    data[3] = position_loop.actual_deg;
    
    // JustFloat 协议帧尾
    uint8_t tail[4] = {0x00, 0x00, 0x80, 0x7f};
    HAL_UART_Transmit(&huart1, (uint8_t*)data, sizeof(data), 10);
    HAL_UART_Transmit(&huart1, tail, 4, 10);
}
```

VOFA+ 中选择 JustFloat 协议，即可实时绘制波形，用于 PID 参数整定和故障诊断。

## 注意事项

- **电流采样时机**：ADC 触发点应设在 PWM 下溢点（或上溢点），避开 MOSFET 开关噪声。
- **死区时间**：若使用 X-NUCLEO-IHM07M1 等集成驱动板，其内部栅极驱动器已处理死区，MCU 无需配置互补输出和死区。
- **参数辨识**：电机相电阻 `Rs` 和电感 `Ls` 的准确值影响 PI 参数整定效果，可通过离线辨识获得。
- **保护机制**：务必实现过流、过压和堵转保护，调试时从低压小电流开始。

## 参考资料

- [STM32 Motor Control Knowledge Database - ST 官方](https://wiki.stmicroelectronics.cn/stm32mcu/wiki/Category:Motor_Control)
