# **C++ 开发规范文档**

## **1. C++ 标准**

* 使用 **C++17**
* 启用编译器的警告和警告等级设置，以确保代码质量高。

  ```cpp
  -Wall -Wextra -Werror
  ```

---

## **2. 编码风格**

### **2.1 变量命名**

* 使用 **驼峰命名法**（CamelCase）为变量命名，除非它是常量或宏。

  * **例**：`int employeeId;`
  * **常量命名**：全大写字母并使用下划线分隔。

    ```cpp
    const int MAX_RETRY_COUNT = 3;
    ```

* 使用 **前缀约定**：

  * 类成员变量使用 `m_` 前缀。例如，`m_employeeId`。
  * 静态成员变量使用 `s_` 前缀。例如，`s_defaultEmployeeId`。
  * 指针变量使用 `p_` 前缀。例如，`pEmployee`。

### **2.2 函数命名**

* 使用 **动词+名词** 的格式，采用 **驼峰命名法**。

  ```cpp
  void processEmployeeData();  // 正确
  void process_employee_data();  // 错误
  ```

* 函数名应简洁且具描述性，不应含有过多的缩写。

### **2.3 结构体和类命名**

* 类名和结构体名采用 **首字母大写** 的 **驼峰命名法**（PascalCase）。

  ```cpp
  class EmployeeManager; // 正确
  struct EmployeeData;   // 正确
  ```

* 结构体成员变量使用 `m_` 前缀。

### **2.4 常量与宏**

* 宏定义全部使用 **大写字母**，并用下划线分隔单词。

  ```cpp
  #define MAX_BUFFER_SIZE 1024
  ```

### **2.5 类型定义**

* 避免使用 `typedef`，推荐使用 `using` 关键字。

  ```cpp
  using EmployeeList = std::vector<Employee>;
  ```

---

## **3. 智能指针使用规范**

* **智能指针优先于裸指针**，使用 `std::unique_ptr` 和 `std::shared_ptr`，仅在必要时使用 `std::weak_ptr`。
* **优先使用 `std::unique_ptr`** 来管理资源所有权，它表示对象的唯一所有者，不会造成多余的资源管理问题。

  ```cpp
  std::unique_ptr<Employee> employee = std::make_unique<Employee>();
  ```

* **`std::shared_ptr`** 应在多个对象共享资源时使用，但应尽量避免不必要的共享指针，避免循环引用。

  ```cpp
  std::shared_ptr<Employee> employee = std::make_shared<Employee>();
  ```

* 永远避免直接使用裸指针管理资源，如果资源不再需要，使用智能指针的自动析构。

---

## **4. 接口设计规范**

* **接口应简洁**：接口应该只提供必要的功能，不要将不相关的功能混合在接口中。
* **接口方法名应清晰且具描述性**，每个方法的功能应明确，避免方法名过长或过于复杂。
* **接口类应避免包含数据**，只提供方法声明，保持接口的纯粹性。

  ```cpp
  class IDataProvider {
  public:
      virtual void fetchData() = 0;
      virtual bool isAvailable() = 0;
      virtual ~IDataProvider() = default;
  };
  ```

---

## **5. 编译和构建工具**

* 使用 **CMake** 作为项目的构建工具。

  * 项目配置文件 (`CMakeLists.txt`) 应该包含目标平台支持、编译器选项、库链接等配置。
  * **GCC** 或 **Clang** 编译器推荐配置：使用 `-Wall -Wextra -Werror` 选项来启用详细警告并将其视为错误。
* **使用 Ninja 构建系统**，可并行化构建过程，提高编译速度。
* 确保所有目标都正确地指定了输出目录：

  ```cmake
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  ```

---

## **6. 依赖管理工具**

* 使用 **vcpkg** 或 **Conan** 来管理第三方依赖库，确保依赖项的版本控制。
* 在 `CMakeLists.txt` 文件中明确列出项目的依赖。

  ```cmake
  find_package(Boost REQUIRED)
  find_package(Spdlog REQUIRED)
  ```

* 使用合适的版本控制来确保项目的依赖性一致性。若使用 vcpkg 或 Conan，确保通过 `CMake` 正确引入相关库。

---

## **7. 代码质量和测试**

* 使用 **单元测试框架**（如 Google Test 或 Catch2）来保证代码质量和功能验证。
* 在每个功能模块中写测试，并使用 `enable_testing()` 来启用 CMake 测试支持。
* **代码覆盖率**：使用工具（如 `gcov` 或 `lcov`）检测测试的覆盖率，确保没有未覆盖的代码。

---

## **8. 文档和注释规范**

* **函数和类的文档注释**：使用 Doxygen 格式，为每个函数、类、方法提供详细的文档说明。

  ```cpp
  /**
   * @brief 计算员工薪资。
   * 
   * 该函数计算员工的月薪，包括基本工资、奖金和其他补贴。
   * 
   * @param baseSalary 基本工资。
   * @param bonus 奖金。
   * @return 返回员工的总薪资。
   */
  double calculateSalary(double baseSalary, double bonus);
  ```

* **避免过度注释**，尽量通过代码本身自解释，不需要过多注释重复简单的操作。

---

## **9. 防止头文件重复包含**

* 使用下面的代码段

```C++
#ifndef CLIENT_CORE_FILEWORKER_H
#define CLIENT_CORE_FILEWORKER_H

#endif
```

来防止头文件重复包含

---

## **10. 项目结构**

* 使用标准的目录结构，使项目组织更清晰：

```mermaid
├── CMakeLists.txt            # CMake 构建文件
├── include                   # 头文件
│   └── project_name
├── src                       # 源文件
│   └── project_name
├── test                      # 测试代码
├── build                     # 编译输出目录
├── libs                      # 第三方库
├── README.md                 # 项目说明文档
└── .gitignore                # Git 忽略文件
```

## **11. 异常处理**

* **避免使用异常进行流程控制**：异常应该用于处理错误情况，而不是常规流程。
* 使用 **标准库的异常类**（如 `std::runtime_error`）来抛出和捕获异常。
* 异常信息应尽可能具体，便于后期调试。

---

## **12. 版本控制规范**

* 使用 **Git** 进行版本控制，并遵循 [Git Flow](https://nvie.com/posts/a-successful-git-branching-model/) 流程。
* 提交信息应该简洁明了，遵循 [Conventional Commits](https://www.conventionalcommits.org/) 规范。

---

## **13. 性能优化**

* **避免不必要的内存分配和拷贝操作**，使用 **移动语义**（Move Semantics）来提高效率。
