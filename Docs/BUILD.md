# 构建指南

本文档详细说明如何在不同平台上构建ChatAndFileTransferSystem项目。

## 目录

- [环境要求](#环境要求)
- [安装依赖](#安装依赖)
- [构建项目](#构建项目)
- [常见问题](#常见问题)

## 环境要求

### Windows

- **操作系统**: Windows 10 或更高版本
- **编译器**: 
  - MinGW 7.3+ (推荐)
  - MSVC 2019+ (Visual Studio 2019或更高版本）
- **CMake**: 3.16 或更高版本
- **Qt**: Qt 6.2 或更高版本（推荐Qt 6.5+）

### Linux

- **操作系统**: Ubuntu 20.04+ 或其他主流Linux发行版
- **编译器**: 
  - GCC 9+ (推荐)
  - Clang 10+
- **CMake**: 3.16 或更高版本
- **Qt**: Qt 6.2 或更高版本（推荐Qt 6.5+）

### macOS

- **操作系统**: macOS 10.15 (Catalina) 或更高版本
- **编译器**: Clang 12+ (Xcode 12+)
- **CMake**: 3.16 或更高版本
- **Qt**: Qt 6.2 或更高版本（推荐Qt 6.5+）

## 安装依赖

### Windows

#### 1. 安装Qt

1. 访问 [Qt官网](https://www.qt.io/download)
2. 下载Qt 6.5或更高版本的在线安装程序
3. 运行安装程序，选择以下组件：
   - Qt 6.5.x
   - MinGW 11.2.0 64-bit (或MSVC 2019 64-bit）
   - Qt Creator (可选）
4. 安装完成后，设置环境变量：
   ```cmd
   set Qt_DIR=C:\Qt\6.5.x\mingw_64\lib\cmake\Qt6
   set PATH=%PATH%;C:\Qt\6.5.x\mingw_64\bin
   ```

#### 2. 安装CMake

1. 访问 [CMake官网](https://cmake.org/download/)
2. 下载Windows x64 Installer
3. 运行安装程序，按照默认设置安装
4. 将CMake添加到PATH环境变量

#### 3. 安装Git（可选）

1. 访问 [Git官网](https://git-scm.com/downloads)
2. 下载Windows版本
3. 运行安装程序，按照默认设置安装

### Linux (Ubuntu)

#### 1. 安装基础依赖

```bash
sudo apt update
sudo apt install -y build-essential cmake git
```

#### 2. 安装Qt

```bash
# 方法1: 使用apt安装（版本可能较旧）
sudo apt install -y qt6-base-dev qt6-tools-dev

# 方法2: 从官网安装（推荐）
# 访问 https://www.qt.io/download
# 下载Linux版本的在线安装程序
chmod +x qt-unified-linux-x64-online.run
sudo ./qt-unified-linux-x64-online.run
```

#### 3. 安装MySQL和Redis（可选，仅用于运行）

```bash
sudo apt install -y mysql-server redis-server
```

### macOS

#### 1. 安装Homebrew（如果未安装）

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

#### 2. 安装依赖

```bash
brew install cmake git qt@6 mysql redis
```

#### 3. 设置Qt路径

```bash
echo 'export PATH="/opt/homebrew/opt/qt@6/bin:$PATH"' >> ~/.zshrc
echo 'export CMAKE_PREFIX_PATH="/opt/homebrew/opt/qt@6:$CMAKE_PREFIX_PATH"' >> ~/.zshrc
source ~/.zshrc
```

## 构建项目

### Windows

#### 使用构建脚本（推荐）

1. 打开命令提示符（cmd）或PowerShell
2. 进入项目根目录
3. 运行构建脚本：

```cmd
# Release构建
Scripts\build.bat

# Debug构建
Scripts\build.bat debug
```

#### 手动构建

1. 打开命令提示符（cmd）或PowerShell
2. 进入项目根目录
3. 创建构建目录：

```cmd
mkdir build
cd build
```

4. 配置CMake：

```cmd
# MinGW
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# MSVC
cmake .. -G "Visual Studio 16 2019" -A x64
```

5. 编译项目：

```cmd
# MinGW
mingw32-make -j%NUMBER_OF_PROCESSORS%

# MSVC
cmake --build . --config Release
```

### Linux

#### 使用构建脚本（推荐）

1. 打开终端
2. 进入项目根目录
3. 给脚本添加执行权限：

```bash
chmod +x Scripts/*.sh
```

4. 运行构建脚本：

```bash
# Release构建
./Scripts/build.sh

# Debug构建
./Scripts/build.sh debug
```

#### 手动构建

1. 打开终端
2. 进入项目根目录
3. 创建构建目录：

```bash
mkdir build
cd build
```

4. 配置CMake：

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

5. 编译项目：

```bash
make -j$(nproc)
```

### macOS

#### 使用构建脚本（推荐）

1. 打开终端
2. 进入项目根目录
3. 给脚本添加执行权限：

```bash
chmod +x Scripts/*.sh
```

4. 运行构建脚本：

```bash
# Release构建
./Scripts/build.sh

# Debug构建
./Scripts/build.sh debug
```

#### 手动构建

1. 打开终端
2. 进入项目根目录
3. 创建构建目录：

```bash
mkdir build
cd build
```

4. 配置CMake：

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

5. 编译项目：

```bash
make -j$(sysctl -n hw.ncpu)
```

## 构建选项

### CMake选项

在配置CMake时，可以使用以下选项：

```bash
# 构建所有模块（默认）
cmake .. -DBUILD_CLIENT=ON -DBUILD_METASERVER=ON -DBUILD_STORAGESERVER=ON -DBUILD_TESTS=ON

# 只构建客户端
cmake .. -DBUILD_CLIENT=ON -DBUILD_METASERVER=OFF -DBUILD_STORAGESERVER=OFF

# 只构建服务器
cmake .. -DBUILD_CLIENT=OFF -DBUILD_METASERVER=ON -DBUILD_STORAGESERVER=ON

# 不构建测试
cmake .. -DBUILD_TESTS=OFF
```

### 构建类型

- **Release**: 优化构建，用于生产环境
- **Debug**: 调试构建，包含调试符号
- **RelWithDebInfo**: 带调试信息的优化构建

## 清理构建

### Windows

```cmd
Scripts\clean.bat
```

### Linux/macOS

```bash
./Scripts/clean.sh
```

## 运行测试

### Windows

```cmd
Scripts\test.bat
```

### Linux/macOS

```bash
./Scripts/test.sh
```

## 常见问题

### Q: CMake找不到Qt6？

**A**: 请确保Qt6已正确安装，并设置Qt_DIR环境变量。

**Windows**:
```cmd
set Qt_DIR=C:\Qt\6.5.x\mingw_64\lib\cmake\Qt6
```

**Linux/macOS**:
```bash
export Qt_DIR=/path/to/Qt/6.5.x/gcc_64/lib/cmake/Qt6
```

### Q: 编译时提示找不到头文件？

**A**: 请确保已正确配置CMake，并且所有依赖项都已安装。

检查构建目录中的CMakeCache.txt，确认所有路径都正确。

### Q: 链接时提示找不到库？

**A**: 请确保所有依赖库都已正确安装，并且CMake能够找到它们。

可以使用`cmake --trace`查看详细的构建过程。

### Q: Windows下MinGW编译失败？

**A**: 请确保MinGW版本与Qt版本匹配。

推荐使用Qt安装程序自带的MinGW编译器。

### Q: Linux下Qt模块找不到？

**A**: 请确保已安装所有必需的Qt模块。

```bash
sudo apt install -y qt6-base-dev qt6-tools-dev qt6-network-dev qt6-sql-dev
```

### Q: macOS下编译失败？

**A**: 请确保Xcode命令行工具已安装。

```bash
xcode-select --install
```

### Q: 如何指定Qt安装路径？

**A**: 可以在配置CMake时指定Qt路径。

```bash
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt/6.5.x/gcc_64
```

### Q: 如何加速编译？

**A**: 可以使用多线程编译。

**Windows**:
```cmd
mingw32-make -j8
```

**Linux/macOS**:
```bash
make -j8
```

### Q: 如何生成安装包？

**A**: 可以使用CMake的install目标。

```bash
cmake --build . --target install
```

或者使用部署脚本：

**Windows**:
```cmd
Scripts\deploy.bat
```

**Linux/macOS**:
```bash
./Scripts/deploy.sh
```

### Q: 如何查看详细的编译输出？

**A**: 可以使用CMake的详细模式。

```bash
cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
```

或者在编译时使用详细模式：

```bash
make VERBOSE=1
```

## 高级构建选项

### 使用Ninja构建系统

Ninja是一个快速的构建系统，可以显著加快编译速度。

#### 安装Ninja

**Windows**:
```cmd
choco install ninja
```

**Linux/macOS**:
```bash
brew install ninja
```

#### 使用Ninja构建

```bash
cmake .. -G Ninja
ninja
```

### 使用ccache加速编译

ccache是一个编译缓存工具，可以加速重复编译。

#### 安装ccache

**Windows**:
```cmd
choco install ccache
```

**Linux**:
```bash
sudo apt install -y ccache
```

**macOS**:
```bash
brew install ccache
```

#### 配置ccache

在CMakeLists.txt中添加：

```cmake
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()
```

## 获取帮助

如果遇到构建问题，请：

1. 查看本文档的[常见问题](#常见问题)部分
2. 查看项目Issues: https://github.com/yourusername/ChatAndFileTransferSystem/issues
3. 提交新的Issue，包含以下信息：
   - 操作系统版本
   - 编译器版本
   - CMake版本
   - Qt版本
   - 完整的错误信息

## 下一步

构建成功后，请查看[部署指南](DEPLOYMENT.md)了解如何部署和运行项目。
