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
