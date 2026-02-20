# CppTest Demo 集合

这个项目用于实践和沉淀一些小型 Demo，重点是“快速验证 + 可独立构建”。

## 项目目标

- 用最小成本验证想法、语法特性和工程实践。
- 支持混合语言（例如 Java / C++ / C）。
- 每个 Demo 独立成目录，互不影响，便于单独编译和运行。
- 每个 Demo 都配有 `README.md`，说明用途、构建方式和运行方法。

## 目录约定

建议按如下方式组织：

```text
CppTest/
├── README.md                  # 项目总说明（当前文件）
└── demos/
    ├── demo-name-a/
    │   ├── README.md          # 该 demo 的说明
    │   ├── src/               # 源码（按语言自定义）
    │   └── build/             # 可选：构建产物目录（可加入 .gitignore）
    └── demo-name-b/
        ├── README.md
        └── ...
```

## 单个 Demo 规范（建议）

每个 Demo 目录建议包含：

- `README.md`：说明 demo 在做什么、为什么做。
- 源代码：按语言习惯组织（如 `src/`、`include/`、`main.c`、`Main.java` 等）。
- 构建命令：在 README 中明确给出（例如 `g++`、`cmake`、`javac`）。
- 运行命令：给出最小可复现步骤。

## 说明

这个仓库不是单一应用，而是多个独立实验的集合。  
后续会逐步补充各语言 Demo 目录和对应文档。

## 当前 Demo

- `demos/core_dump_cpp`：C++ + CMake 的 core dump 触发示例（A/B 继承与析构崩溃）。
- `demos/dlopen_symbol_collision`：`dlopen` 顺序加载两个共享库，验证同名全局变量/函数的符号冲突与解析行为。
- `demos/singleton_cpp`：C++ 单例写法合集（静态局部变量、堆区 + `call_once`），并包含导出 `.so` 时的进程级单例安全示例。
- `demos/cpp_std_lab`：C++ 标准实验台（第一版），对比 C++17/20/23 下 `nth_element` 的可用性与结果一致性（`ranges` / fallback）。
