# ERDP

Embedded Rapid Development Platform

## 简介

ERDP（嵌入式快速开发平台）是一个基于 C++ 构建的通用单片机开发框架，核心目标是实现软硬件解耦，提升开发效率、降低维护成本。

**关键特性：**
- 统一的硬件抽象层（HAL）
- 跨芯片平台复用（STM32F4XX / GD32F4XX）
- 面向对象 C++ 接口

**ERDP：让嵌入式开发，像软件一样简单。**

---

## 文件结构

```
ERDP/
├── CMakeLists.txt              # 根构建配置
├── CMakePresets.json           # 编译预设配置
├── .vscode/                   # VS Code 配置
│   ├── launch.json            # 调试配置
│   ├── tasks.json             # 构建任务
│   └── settings.json          # 编辑器设置
├── cmake/                     # CMake 辅助脚本
├── Scripts/
│   └── OpenOCD/              # OpenOCD 配置文件
├── Source/
│   ├── Application/           # 应用程序入口
│   ├── Service/               # 服务层 (Log)
│   ├── Library/               # 库层 (st7789, printf)
│   ├── HAL/                   # 硬件抽象层 (GPIO, UART, SPI, I2C, EXTI)
│   ├── OSAL/                  # 操作系统抽象层
│   ├── Interface/             # 平台接口层
│   │   ├── Drivers/           # 外设驱动适配 (STM32F4XX, GD32F4XX)
│   │   └── RTOS/              # RTOS 接口适配
│   ├── Kernel/                # 内核层
│   │   ├── Drivers/           # 芯片厂商驱动
│   │   └── RTOS/              # FreeRTOS
│   ├── Board/                 # 板级支持包
│   ├── Common/                # 公共头文件
│   └── Test/                  # 测试代码
└── build/                     # 构建输出目录
```

## 快速开始

👉 [快速开始指南](./QuickStart.md)

## 架构设计

👉 [ERDP 架构文档](./Architecture.md)

## 支持的芯片

| 芯片系列 | 驱动路径 |
|----------|----------|
| STM32F4XX | Source/Kernel/Drivers/STM32F4XX |
| GD32F4XX | Source/Kernel/Drivers/GD32F4XX |

## 许可证

MIT License
