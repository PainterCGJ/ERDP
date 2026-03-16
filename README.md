# ERDP
Embedded rapid development platform

## 简介
ERDP（嵌入式快速开发平台）是一个基于C++构建的通用单片机开发框架。其**核心目标**是实现软硬件的彻底解耦，从而**提升开发效率、降低维护成本**。

**关键特性：**
- **统一的硬件抽象层（HAL）**：将芯片特定外设（如GPIO、UART、I2C）封装为标准化API，使应用代码与底层硬件隔离。
- **面向软件工程师的友好设计**：通过提供清晰的、面向对象的C++接口，屏蔽底层硬件的复杂性，让不具备深厚硬件背景的开发者也能快速上手并实现功能。
- **高度的可移植性与可复用性**：基于本平台编写的应用程序，可以无缝地在不同架构的单片机间迁移，极大提升了代码的复用性和项目迭代速度。

ERDP（嵌入式快速开发平台）致力于解决嵌入式开发中固有的软硬件耦合难题。我们通过构建一个标准化的C++开发框架，将复杂的硬件细节封装成易于理解的软件接口。这不仅显著提高了开发效率，更打破了技术的壁垒，让软件开发者能够专注于业务逻辑与创新，无需深入底层即可快速实现强大的单片机应用。

**ERDP：让嵌入式开发，像软件一样简单。**

**传统嵌入式开发的痛点：**
- 应用代码与芯片绑定，更换硬件成本高昂。
- 开发门槛高，软件工程师需深入钻研数据手册。
- 代码复用性差，项目维护困难。

**ERDP的解决方案：**
ERDP（嵌入式快速开发平台）是一个创新的C++框架，它通过**硬件抽象化**和**模块组件化**设计，实现了：
- **对软件开发者**：提供清晰的、高级别的C++ API，让您像操作软件对象一样控制硬件，快速构建应用，无需关心底层寄存器配置。
- **对项目与团队**：建立开发标准，提升代码复用率和可维护性，使项目能在不同硬件平台间平滑迁移，最终达成**降低开发成本、加速产品上市**的核心目标。

---

## 开发环境搭建（Windows）

本节介绍在 Windows 系统中搭建 ERDP 开发环境所需的软件工具。

### 所需软件清单

| 软件 | 用途 | 下载地址 |
|------|------|----------|
| **ARM GCC 工具链** | ARM Cortex-M 交叉编译工具链 | [Arm GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) |
| **CMake** | 跨平台构建系统生成工具 | [CMake Downloads](https://cmake.org/download/) |
| **Ninja** | 高效构建工具（比 Make 更快） | [Ninja Build](https://github.com/ninja-build/ninja/releases) |
| **OpenOCD** | 开源烧录调试工具 | [OpenOCD Downloads](https://gnutoolchains.com/arm-eabi/openocd/) |
| **Git for Windows** | 版本控制工具 | [Git Downloads](https://git-scm.com/download/win) |
| **VS Code**（可选） | 代码编辑器 + 开发环境 | [VS Code](https://code.visualstudio.com/) |

### 安装步骤

#### 1. 安装 ARM GCC 工具链

1. 访问 [Arm GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) 下载页面
2. 选择 **Windows x64** 版本的 `arm-none-eabi` 工具链
3. 运行安装程序，建议安装到较短路径，如 `D:\Tools\arm-gnu-toolchain`
4. **重要**：将工具链的 `bin` 目录添加到系统 PATH 环境变量
   - 例如：`D:\Tools\arm-gnu-toolchain\bin`

安装完成后，在命令行验证：
```bash
arm-none-eabi-gcc --version
```

#### 2. 安装 CMake

1. 访问 [CMake 下载页面](https://cmake.org/download/)
2. 下载 Windows x64 Installer (`cmake-*-windows-x86_64.msi`)
3. 运行安装程序，**勾选 "Add CMake to the system PATH"**
4. 建议安装到 `D:\Tools\CMake`

安装完成后，在命令行验证：
```bash
cmake --version
```

#### 3. 安装 Ninja

1. 访问 [Ninja GitHub Releases](https://github.com/ninja-build/ninja/releases)
2. 下载 `ninja-win.zip`（或 `ninja.exe`）
3. 将 `ninja.exe` 复制到 `D:\Tools\ninja` 目录
4. 将该目录添加到系统 PATH 环境变量

安装完成后，在命令行验证：
```bash
ninja --version
```

#### 4. 安装 OpenOCD

1. 访问 [GnuToolchains.com](https://gnutoolchains.com/arm-eabi/openocd/) 下载 OpenOCD
2. 下载后解压到 `D:\Tools\openocd`
3. 将 `bin` 目录添加到系统 PATH 环境变量
   - 例如：`D:\Tools\openocd\bin`

或者使用 MSYS2 安装：
```bash
pacman -S mingw-w64-x86_64-openocd
```

安装完成后，在命令行验证：
```bash
openocd --version
```

#### 5. 安装 Git for Windows

1. 访问 [Git 下载页面](https://git-scm.com/download/win)
2. 运行安装程序
3. 建议选项：
   - 使用 VS Code 作为默认编辑器
   - 勾选 "Git LFS"
   - 勾选 "Associate .sh files to be run with Bash"

#### 6. 配置环境变量

在系统环境变量中配置以下路径（以安装到 `D:\Tools` 为例）：

```
ARM_GNU_PATH=D:\Tools\arm-gnu-toolchain
PATH中添加:
  D:\Tools\arm-gnu-toolchain\bin
  D:\Tools\CMake\bin
  D:\Tools\ninja
  D:\Tools\openocd\bin
```

**注意**：ERDP 项目通过 CMake 变量 `ARM_GNU_PATH` 指定工具链路径，你也可以：
- 在 CMake 配置时通过 `-DARM_GNU_PATH=...` 指定
- 或设置环境变量 `ARM_GNU_PATH`

### 项目结构

```
ERDP/
├── CMakeLists.txt           # 项目根构建配置
├── cmake/                   # CMake 辅助脚本
│   ├── flash_by_install.cmake
│   └── flash_script.cmake
├── Scripts/
│   └── OpenOCD/             # OpenOCD 配置文件
├── Source/
│   ├── Application/        # 应用程序入口
│   ├── Common/             # 通用代码
│   ├── HAL/                # 硬件抽象层
│   │   ├── GPIO/
│   │   └── UART/
│   ├── Interface/         # 接口层
│   ├── Kernel/            # 内核驱动
│   ├── OSAL/              # 操作系统抽象层
│   ├── Service/           # 业务服务层
│   └── Test/              # 测试代码
├── build/                  # 构建输出目录
└── README.md
```

### 快速开始

#### 1. 克隆项目

```bash
git clone https://github.com/your-repo/erdp.git
cd erdp
```

#### 2. 配置项目

使用 CMake 配置构建系统（推荐使用 Ninja）：

```bash
cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DARM_GNU_PATH=D:/Tools/arm-gnu-toolchain ..
```

或者使用 CMake GUI 工具进行配置。

#### 3. 编译项目

编译所有目标：
```bash
cmake --build .
```

编译特定目标（如 app）：
```bash
cmake --build . --target app
```

#### 4. 烧录固件

将固件烧录到目标设备：
```bash
cmake --install . --prefix flash
```

或者手动使用 OpenOCD 烧录：
```bash
openocd -f Scripts/OpenOCD/openocd_stm32f4_stlink.cfg -c "program build/flash/flash.elf verify reset; shutdown"
```

### 支持的芯片

当前 ERDP 支持以下芯片系列：

| 芯片系列 | 驱动路径 | 配置文件 |
|----------|----------|----------|
| STM32F4XX | Source/Kernel/Drivers/STM32F4XX | openocd_stm32f4_stlink.cfg |
| GD32F4XX | Source/Kernel/Drivers/GD32F4XX | openocd_gd32f470_stlink.cfg |

### 常见问题

**Q: 编译时提示找不到 arm-none-eabi-gcc？**
> A: 请确认 ARM GCC 工具链已正确安装，并将 `bin` 目录添加到 PATH 环境变量。

**Q: CMake 配置失败，提示 Ninja 未找到？**
> A: 请确保 Ninja 已安装并添加到 PATH 环境变量，然后重新运行 CMake 配置。

**Q: 烧录失败，OpenOCD 无法连接设备？**
> A: 请检查：
> 1. 调试器（ST-Link/JLink）驱动是否正确安装
> 2. OpenOCD 配置文件是否与你的硬件匹配
> 3. 目标板是否正常供电

**Q: 如何切换不同的芯片型号？**
> A: 在 CMake 配置时指定 DRIVER_PATH：
> ```bash
> cmake -G Ninja -DDRIVER_PATH=Source/Kernel/Drivers/GD32F4XX ..
> ```

### 后续规划

- [ ] 添加 Linux/macOS 开发环境搭建教程
- [ ] 完善 API 文档
- [ ] 增加更多芯片支持（STM32F1xx, STM32H7xx 等）
- [ ] 添加单元测试框架集成
- [ ] 开发 IDE 插件支持

---

## 许可证

MIT License
