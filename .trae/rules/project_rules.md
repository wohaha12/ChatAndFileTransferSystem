客户端项目规则（基于 Qt 5.14.2 + C++17 + QMake）
1. 环境与构建标准
基础标准：遵循 C++17 核心规范，兼容 MinGW 7.3 编译器。
构建系统：统一使用 QMake，项目文件 .pro 必须明确开启 C++17 支持：CONFIG += c++17
QT += core gui widgets network # 按需添加模块
2. 内存管理与所有权（Qt 特化）
Qt 对象：继承自 QObject 的类（如 QWidget），使用 Qt 对象树 机制管理内存（即指定 parent）。禁止对 Qt 对象使用 std::unique_ptr 或 delete，除非对象没有父对象且需要独占所有权。
非 Qt 数据结构：业务逻辑层的数据结构（Model、POD），严格遵循之前的 C++17 规范，使用 std::unique_ptr 或 std::shared_ptr，禁止随意 new/delete。
原始指针：仅用于指向“非所有权”的 Qt 对象（如 QObject::parent() 返回值或信号槽参数）。
3. 信号与槽与异步
连接语法：严禁使用旧式 SIGNAL() / SLOT() 宏字符串语法（编译期不检查）。
推荐语法：使用函数指针语法或 Lambda 表达式。// 推荐
connect(button, &QPushButton::clicked, this, &MyClass::onButtonClicked);
// 简单逻辑可用 Lambda
connect(button, &QPushButton::clicked, this, [](){ /* ... */ });
多线程：使用 QThread、QRunnable 配合信号槽进行跨线程通信，禁止在子线程直接操作 UI。
4. 类型与接口
字符串处理：
UI 层：统一使用 QString。
核心逻辑层：建议使用 std::string 并与 UI 层隔离，转换接口应明确。
注意：Qt 5.14 尚未引入 QStringView，传递只读字符串参数请使用 const QString&。
容器：涉及 Qt API 的交互使用 QList/QVector；内部算法处理优先使用 std::vector 以提升性能。