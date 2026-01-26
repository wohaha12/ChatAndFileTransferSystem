# ChatAndFileTransferSystem 客户端项目

## 项目概述
这是一个基于Qt 5.14.2开发的聊天与文件传输系统客户端，采用C++17标准，支持跨平台运行。

## 技术栈
- Qt 5.14.2 (Core, Gui, Widgets, Network)
- C++17
- QMake构建系统

## 项目结构
```
Client/
├── Core/              # 核心业务逻辑
│   ├── FileWorker.cpp/h
│   └── TransferManager.cpp/h
├── Models/            # 数据模型
│   ├── FileModel.cpp/h
│   └── UserModel.cpp/h
├── Network/           # 网络通信
│   ├── MetaConnection.cpp/h
│   ├── NetworkManager.cpp/h
│   └── StorageConnection.cpp/h
├── UI/               # 用户界面
│   ├── MainWindow.cpp/h/ui
│   ├── LoginDialog.cpp/h/ui
│   ├── ChatWidget.cpp/h/ui
│   ├── FileWidget.cpp/h/ui
│   ├── StyleManager.cpp/h
│   └── AnimationManager.cpp/h
├── Utils/             # 工具类
│   └── ConfigManager.cpp/h
├── images/            # 资源文件
├── images.qrc         # Qt资源文件
├── main.cpp           # 程序入口
├── Client.pro          # QMake项目文件
└── Info.plist         # macOS应用信息
```

## 功能特性
- 现代化UI设计，支持响应式布局
- 用户登录/注册功能
- 实时聊天功能
- 文件传输与管理
- 好友列表管理
- 搜索功能
- 动画效果与交互体验

## 构建说明

### 使用Qt Creator
1. 打开Qt Creator
2. 选择"打开项目"
3. 导航到Client目录，选择Client.pro文件
4. 配置构建套件（Kit）
5. 点击"构建"按钮或按Ctrl+B

### 使用命令行
```bash
# 进入项目目录
cd e:/my_qt_project/ChatAndFileTransferSystem/Client

# 生成Makefile
qmake Client.pro

# 编译项目
make  # Linux/macOS
mingw32-make  # Windows MinGW
nmake  # Windows MSVC
```

### 构建配置
- Debug模式：调试版本，包含调试信息
- Release模式：发布版本，优化编译

## 平台支持
- Windows 10/11 (MinGW 7.3+ / MSVC 2019+)
- Linux (GCC 7.5+)
- macOS 10.13+ (Clang 10.0+)

## 依赖项
- Qt 5.14.2 或更高版本
- C++17 兼容编译器

## 注意事项
1. 确保已安装Qt 5.14.2或更高版本
2. Windows系统需要安装对应的运行时库
3. macOS需要正确的开发者证书签名（发布版本）
4. Linux系统可能需要安装额外的依赖库

## 许可证
[在此添加许可证信息]