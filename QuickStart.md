# ERDP Quick Start

本文档按两种开发方式组织：

- CMake（推荐，跨平台、便于自动化）
- MDK（后续补充）

---

## 1. CMake 方式（环境搭建与快速上手）

### 1.1 适用范围

适用于：

- 使用 `arm-none-eabi-gcc/g++` 工具链进行构建
- 使用 `CMake + Ninja` 进行编译管理
- 使用 `OpenOCD` 进行下载与调试

项目已内置 `CMakePresets.json`，支持以下预设：

- `stm32f407vet6+freertos-debug`
- `stm32f407vet6+freertos-release`
- `gd32f470zit6+freertos-debug`
- `gd32f470zit6+freertos-release`

---

### 1.2 Windows 工具链下载与安装

建议统一安装到 `D:\Tools`（或你自己的固定目录），避免路径过长。

| 工具 | 作用 | 推荐版本 | 下载地址 |
|---|---|---|---|
| Arm GNU Toolchain (`arm-none-eabi`) | 交叉编译（C/C++/链接） | 13.x 或更新 | [Arm GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) |
| CMake | 构建系统生成 | 3.20+ | [CMake Downloads](https://cmake.org/download/) |
| Ninja | 高速构建后端 | 最新稳定版 | [Ninja Releases](https://github.com/ninja-build/ninja/releases) |
| OpenOCD | 烧录与调试服务端 | 0.11+（xPack） | [xPack OpenOCD](https://github.com/xpack-dev-tools/openocd-xpack/releases) |
| Git for Windows | 拉取与版本管理 | 最新稳定版 | [Git for Windows](https://git-scm.com/download/win) |

可选开发环境：

- VS Code + CMake Tools + Cortex-Debug

安装后建议目录示例：

- `D:\Tools\arm-gnu-toolchain-13.x\`
- `D:\Tools\CMake\`
- `D:\Tools\ninja\`
- `D:\Tools\xpack-openocd\`

---

### 1.3 环境变量与 PATH 配置（Windows）

ERDP 的 CMake 会优先读取 `ARM_GNU_PATH`，该变量必须指向**工具链根目录**，不是 `bin`。

#### 1.3.1 必须配置的环境变量

- `ARM_GNU_PATH`：如  
  `D:\Tools\arm-gnu-toolchain-13.x`

#### 1.3.2 PATH 需要添加的目录

至少添加以下路径：

- `D:\Tools\arm-gnu-toolchain-13.x\bin`
- `D:\Tools\CMake\bin`
- `D:\Tools\ninja`
- `D:\Tools\xpack-openocd\bin`
- （可选）`C:\Program Files\Git\bin`

#### 1.3.3 图形界面添加方式

1. 打开“系统属性” -> “高级” -> “环境变量”
2. 在“系统变量”中新建：
   - 变量名：`ARM_GNU_PATH`
   - 变量值：你的工具链根目录
3. 编辑 `Path`，逐条新增上述目录
4. 关闭并重新打开终端/VS Code

#### 1.3.4 PowerShell 添加方式（当前用户）

```powershell
[Environment]::SetEnvironmentVariable("ARM_GNU_PATH", "D:\Tools\arm-gnu-toolchain-13.x", "User")

$oldPath = [Environment]::GetEnvironmentVariable("Path", "User")
$addPath = @(
  "D:\Tools\arm-gnu-toolchain-13.x\bin",
  "D:\Tools\CMake\bin",
  "D:\Tools\ninja",
  "D:\Tools\xpack-openocd\bin"
) -join ";"
[Environment]::SetEnvironmentVariable("Path", "$oldPath;$addPath", "User")
```

> 执行后请重开终端，否则新环境变量不会生效。

#### 1.3.5 验证方法

```powershell
echo $env:ARM_GNU_PATH
arm-none-eabi-gcc --version
arm-none-eabi-g++ --version
cmake --version
ninja --version
openocd --version
```

若任一命令提示“找不到”，优先检查对应目录是否已加入 `Path`，以及终端是否已重启。

---

### 1.4 获取代码

```bash
git clone <your-repo-url>
cd ERDP
```

---

### 1.5 配置与编译（推荐 Preset）

> 以下命令在项目根目录执行。

#### 方式 A：先 configure，再 build

```bash
cmake --preset stm32f407vet6+freertos-debug
cmake --build --preset stm32f407vet6+freertos-debug --target app
```

切换到 GD32：

```bash
cmake --preset gd32f470zit6+freertos-debug
cmake --build --preset gd32f470zit6+freertos-debug --target app
```

#### 方式 B：直接 build（前提是对应 preset 已配置过）

```bash
cmake --build --preset stm32f407vet6+freertos-debug --target app
```

---

### 1.6 常用构建目标

与 `.vscode/tasks.json` 一致，常见目标：

- `app`（主示例）
- `hal-test`
- `interface-driver-test`
- `driver-test`
- `osal-test`
- `interface-rtos-test`
- `all`

示例：

```bash
cmake --build --preset stm32f407vet6+freertos-debug --target hal-test
```

---

### 1.7 输出物位置

构建目录规则：

- `build/<preset-name>/...`

常见产物：

- ELF：`build/<preset>/Source/Application/app.elf`
- HEX：`build/<preset>/Source/Application/app.hex`
- 统一调试镜像：`build/flash/flash.elf`（由构建后步骤复制）

---

### 1.8 VS Code 配合（可选）

项目已提供：

- `.vscode/tasks.json`（基于 preset 的构建任务）
- `.vscode/launch.json`（Cortex-Debug 配置）

常见流程：

1. 先执行构建任务（选择 preset + target）
2. 再启动对应调试配置（STM32 或 GD32）

---

### 1.9 常见问题（CMake）

1. 找不到编译器 `arm-none-eabi-gcc`
   - 检查 `ARM_GNU_PATH` 和 `PATH`
2. `cmake --build --preset ...` 报 preset 不存在
   - 检查 `CMakePresets.json` 名称是否拼写一致
3. OpenOCD 连接失败
   - 检查探针驱动、USB 连接、目标板供电和配置文件匹配
4. 更换芯片后仍用旧构建结果
   - 重新执行对应 `cmake --preset <new-preset>` 后再 build

---

## 2. MDK 方式

待补充（后续将补齐 MDK 工程导入、编译、下载与调试流程）。

