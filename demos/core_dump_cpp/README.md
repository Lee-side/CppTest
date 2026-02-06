# core_dump_cpp

这个 Demo 用于验证 C++ 程序崩溃与 core dump 产物。

## 内容说明

- `class B` 继承 `class A`。
- 在 `B` 的析构函数中先演示 `delete nullptr`（这是安全的，不会崩溃）。
- 随后故意对空指针写入，触发段错误（`SIGSEGV`），用于生成 core dump。

> 说明：严格来说，“释放空指针”本身不会崩溃；因此这里额外加入空指针写入来稳定触发 core dump。

## 构建

```bash
cd demos/core_dump_cpp
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## 运行（触发崩溃）

```bash
ulimit -c unlimited
./build/core_dump_demo
```

运行后会在析构阶段崩溃。core 文件位置与系统配置相关：

- Linux 常见在当前目录或由 `/proc/sys/kernel/core_pattern` 指定位置。
- macOS 常见在 `/cores/` 目录（可能需要额外系统配置）。

## macOS 特别注意（核心）

在 macOS 上，仅仅崩溃不一定会落地 `/cores/core.<pid>`。  
实践中，`core_dump_demo` 这类命令行程序可能需要先签名并加上 `get-task-allow`：

```bash
cd build
/usr/libexec/PlistBuddy -c "Add :com.apple.security.get-task-allow bool true" test.entitlements
codesign -s - -f --entitlements test.entitlements core_dump_demo
```

> 如果重复执行 `Add` 提示 key 已存在，可改用：`/usr/libexec/PlistBuddy -c "Set :com.apple.security.get-task-allow true" test.entitlements`

然后再运行：

```bash
ulimit -c unlimited
./core_dump_demo
ls -lt /cores
```

如果仍未看到 `/cores` 下的 core 文件，通常会先在以下目录看到崩溃报告（`.ips`）：

```bash
~/Library/Logs/DiagnosticReports/
```

若没有生成 core 文件，可直接用 `.ips` 做崩溃分析：

```bash
IPS=~/Library/Logs/DiagnosticReports/core_dump_demo-xxxxxx.ips
tail -n +2 "$IPS" | jq -r '
  .threads[] | select(.triggered==true) |
  .frames[] | "\(.symbol)  \(.sourceFile // "?"):\(.sourceLine // 0)"
'
```

可额外查看异常类型：

```bash
tail -n +2 "$IPS" | jq '.exception, .termination'
```

## 生成与现象（示例）

执行：

```bash
ulimit -c unlimited
./build/core_dump_demo
```

预期现象：

1. 程序在 `delete obj;` 后进入 `B::~B()`。
2. `delete nullptr` 正常通过，不会崩溃。
3. 在 `*ptr_ = 42` 处触发 `SIGSEGV`，生成 core（或至少生成 `.ips` 崩溃报告）。

可用下面命令确认：

```bash
ls -lt /cores
ls -lt ~/Library/Logs/DiagnosticReports | head
```

## 栈信息分析（示例）

`lldb` 里常见到类似结果：

```text
* thread #1, stop reason = ESR_EC_DABORT_EL0 (fault address: 0x0)
  * frame #0 ... core_dump_demo`B::~B ... at B.cpp:15:9
    frame #1 ... core_dump_demo`B::~B ... at B.cpp:9:9
    frame #2 ... core_dump_demo`B::~B ... at B.cpp:9:9
    frame #3 ... core_dump_demo`main ... at main.cpp:9:3
```

分析结论：

- `fault address: 0x0` 表示访问了空指针地址。
- 崩溃点在 `B.cpp:15`，即析构函数里的空指针写操作（故意触发）。
- `main.cpp:9` 的 `delete obj;` 触发虚析构链，最终在 `B::~B()` 崩溃。
- 析构函数出现多层栈帧在 C++ 中是正常现象（不同析构变体）。

### 为什么会看到 3 个 `B::~B` 栈帧

这是 C++ ABI 的正常行为。编译器会为析构函数生成多个变体，常见为：

- `D0`：deleting destructor（`delete` 路径使用，最后会释放对象内存）
- `D1`：complete object destructor（完整对象析构）
- `D2`：base object destructor（基类子对象语义析构）

`lldb` 会把它们都显示成 `B::~B()`，所以栈里看起来像有 3 层同名析构。  
这不表示业务代码手写了 3 次析构，也不表示出现了异常递归。

## core 文件很大（现象与原因）

示例现象：

```text
-r--------@ 1 tangjian  admin   4.6G  2  5 10:56 core.99298
```

这在 macOS 上是常见现象，原因通常是：

- core 文件是进程崩溃时的内存镜像，不只是一段调用栈。
- 会包含大量内存映射信息（堆、栈、运行时区域、动态库相关映射等）。
- 即使业务代码很小，进程加载的系统运行时和映射区域也可能让 core 达到 GB 级别。

建议：

- 只做栈定位时，`.ips` 崩溃报告通常已够用。
- 需要深入内存分析时再保留 core，分析完成及时清理 `/cores/core.*`。
- 不需要 core 时可临时关闭：`ulimit -c 0`。

## 调试示例

Linux / gdb:

```bash
gdb ./build/core_dump_demo core
```

macOS / lldb:

```bash
lldb ./build/core_dump_demo -c /cores/core.<pid>
```
