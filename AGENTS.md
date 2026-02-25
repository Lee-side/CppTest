# AGENTS.md - CppTest 项目协作说明

本文件用于让 Codex 在进入仓库后直接获得项目约束，减少每次会话重复初始化说明。

## 1. 适用范围

- 作用域：仓库根目录及全部子目录。
- 项目类型：Demo 集合仓库，不是单一应用。
- 当前语言：以 C++ 为主（后续可扩展多语言）。

## 2. 协作目标

- 优先做“小步快跑、可验证”的改动。
- 每个 Demo 保持独立构建和独立运行，不互相耦合。
- 新增或修改行为时，必须同步更新对应 Demo 的 `README.md`。

## 3. 目录约定（必须遵守）

- 根目录：
  - `README.md`：项目总说明。
  - `demos/`：所有实验 Demo。
- 每个 Demo 目录建议至少包含：
  - `README.md`：用途、构建、运行、验证方式。
  - `src/`：源码。
  - `CMakeLists.txt`：该 Demo 独立构建入口。
  - `build/`：本地构建产物（不提交）。

## 4. Codex 默认工作流程

1. 先定位目标 Demo，只改相关目录，避免跨 Demo 的无关改动。
2. 阅读该 Demo 的 `README.md` 与 `CMakeLists.txt`，确认当前约束。
3. 实现最小改动并保持现有命名和输出风格。
4. 至少执行该 Demo 的构建；若有测试再执行测试。
5. 汇报时给出：
   - 修改了哪些文件；
   - 执行了哪些验证命令；
   - 是否存在未验证项或平台限制。

## 5. 构建与验证命令（优先使用）

### `demos/core_dump_cpp`

```bash
cd demos/core_dump_cpp
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

运行：

```bash
ulimit -c unlimited
./build/core_dump_demo
```

### `demos/dlopen_symbol_collision`

```bash
cd demos/dlopen_symbol_collision
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
cd build
./dlopen_symbol_demo 12
./dlopen_symbol_demo 21
```

### `demos/singleton_cpp`

```bash
cd demos/singleton_cpp
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/singleton_demo
```

### `demos/signal_cpp`

```bash
cd demos/signal_cpp
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/signal_demo
```

### `demos/cpp_std_lab`

```bash
cd demos/cpp_std_lab
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build -V
```

## 6. C++/CMake 约束（对齐现状）

- CMake 最低版本：`3.16`。
- 默认构建类型：`Debug`（若未显式指定）。
- 现有 Demo 主要使用 `C++17`；`cpp_std_lab` 同时覆盖 `17/20/23`。
- 现有可执行/库普遍带 `-g -O0`，新增目标保持一致，除非有明确理由。
- 默认关闭编译器扩展（`CXX_EXTENSIONS OFF`）时，新增目标也应保持一致。

## 7. 修改边界

- 不要无故重命名现有 target、目录或 README 标题。
- 不要引入全局顶层 CMake 破坏“每个 Demo 独立构建”的设计。
- 不要顺手修与任务无关的问题；若发现可疑点，在总结里单独说明即可。
- 未经明确要求，不做破坏性操作（删除大量文件、重置历史等）。

## 8. 新增 Demo 的最小模板

新增目录：`demos/<demo_name>/`

建议最小文件：

- `demos/<demo_name>/README.md`
- `demos/<demo_name>/CMakeLists.txt`
- `demos/<demo_name>/src/main.cpp`（或对应语言入口）

`README.md` 至少包含：

- 这个 Demo 验证什么问题；
- 构建命令；
- 运行命令；
- 预期现象/输出；
- 平台差异（如 Linux/macOS）如果存在。

## 9. 输出与沟通约定

- 默认使用中文回复（用户未指定其他语言时）。
- 结果优先给可执行命令和可复现实验步骤。
- 若命令未执行或受环境限制，明确写出“未验证项”。

