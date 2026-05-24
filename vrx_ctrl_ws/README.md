# vrx_ctrl_ws — USV Motion Control System

基于 ROS2 的无人艇 (WAM-V) 运动控制系统，运行于 VRX (Virtual RobotX) 仿真环境之上。

## 项目架构

```
vrx_ctrl_ws/src/
├── vrx_other_interface/   # 自定义 ROS2 消息接口
├── vrx_data_cpp/          # 传感器数据融合 (C++)
├── vrx_data_py/           # 实验数据记录 (Python)
├── vrx_controller/        # 导航、制导与控制算法 (C++)
├── vrx_experiment/        # 系统辨识实验节点 (C++)
└── vrx_launch/            # 启动文件集合 (Python)
```

## 功能模块

### vrx_other_interface — 自定义消息

| 消息类型 | 字段 | 用途 |
|---------|------|------|
| `Position` | x, y, psi | 二维位姿 |
| `Velocity` | u, v, psi | 船体坐标系速度 |
| `Acceleration` | u, v, psi | 加速度 |
| `State` | time, pos, vel, acc | 完整状态估计 |
| `Losmsg` | error, end | LOS 制导误差与终止标志 |

### vrx_data_cpp — 传感器融合

- **gps_imu_fuse** — 误差状态卡尔曼滤波 (ESKF)，融合 GPS + IMU 数据，估计位置/速度/航向/加速度偏置/角速度偏置 (8维状态)
- **check_sign** — 调试节点，打印 state 与 IMU 原始数据以校验符号一致性
- **fuse_substisute** — 融合替代节点

### vrx_controller — 控制算法

三层级联控制架构：

```
LOS 制导 → 航向控制 (ADRC / PID / Pure Gain) → 底盘角度控制 (Chassis PID) → 推进器
```

- **LOS** (Line-of-Sight) — 基于视线法的路径跟踪制导律，输出期望航向角
- **ADRC** — 自抗扰控制 (Active Disturbance Rejection Control)，含跟踪微分器 + 扩张状态观测器 + 非线性误差反馈
- **PID** — 经典 PID 航向控制器
- **Pure_Gain** — 纯比例增益控制器 (用于对比实验)
- **ch_PID** — 底盘推进器转角 PID 控制器 (含 ±15° 限幅)
- **go_ahead** — 定速直航开环控制

### vrx_experiment — 系统辨识实验

用于获取 USV 动力学模型参数：

- **SysIden** — 随机推力/转角输入，激发系统动态
- **step_speed_up** — 阶跃推力响应测试
- **slow_speed_up** — 斜坡增速测试
- **measure_joint_T** — 推进器转角时间常数测量
- **round** — 定速回转实验
- **Fang** — 方波操舵实验

### vrx_data_py — 数据记录

Python 节点，订阅 topic 并保存实验数据为 `.txt` 文件：

| 节点 | 保存内容 |
|------|---------|
| `Data_Saver` | state + joint + error 全量数据 |
| `Gps_Saver` / `Imu_Saver` | 原始 GPS/IMU 数据 |
| `JointState_Saver` | 关节状态 |
| `Input_Saver` | 控制输入 |
| `IRLS1_Saver` / `IRLS2_Saver` | 迭代重加权最小二乘辨识数据 |
| `Check_Fuse_Saver` | 融合校验数据 |
| `Data2_Saver` | 无误差数据 (对比实验) |

### vrx_launch — 启动文件

| 文件 | 功能 |
|------|------|
| `Ctrl_ADRC.launch.py` | 启动 ADRC 控制全栈 (LOS + ADRC + ch_PID + 融合 + 记录 + 仿真) |
| `Ctrl_PID.launch.py` | 启动 PID 控制全栈 |
| `Ctrl_Gain.launch.py` | 启动纯增益控制全栈 |
| `SysIden_Straight.launch.py` | 直航系统辨识 |
| `SysIden_Other.launch.py` | 其他工况辨识 |
| `Cov_Gps.launch.py` / `Cov_Imu.launch.py` | 传感器协方差标定 |
| `Check_Sign.launch.py` | 符号校验 |
| `Round.launch.py` | 回转实验 |
| `Measure_*.launch.py` | 参数测量实验 |

## 依赖

- ROS2 Humble (rclcpp, rclpy, sensor_msgs, std_msgs, rosgraph_msgs)
- Gazebo Garden (VRX 仿真环境, `vrx_gz` 包)
- Eigen3
- NumPy (Python 数据记录)

## 编译

```bash
cd vrx_ctrl_ws
colcon build --symlink-install
source install/setup.bash
```

## 运行

```bash
# ADRC 控制 (含仿真环境)
ros2 launch vrx_launch Ctrl_ADRC.launch.py

# PID 控制
ros2 launch vrx_launch Ctrl_PID.launch.py

# 系统辨识实验
ros2 launch vrx_launch SysIden_Straight.launch.py
```

## 数据流

```
GPS + IMU ──→ gps_imu_fuse ──→ /state ──→ LOS ──→ /LOS_Angle ──→ ADRC/PID ──→ /Chessis_Input ──→ ch_PID ──→ thruster pos/force
                                    │                                     │
                                    └──→ Data_Saver (logging) ←───────────┘
```

## 参考

- VRX 仿真环境: `vrx_ws/src/vrx_gz`
- ADRC 算法源自无名科创开源飞控 (武汉科技大学)
- Fossen, T.I. "Handbook of Marine Craft Hydrodynamics and Motion Control" (LOS 制导参考)
