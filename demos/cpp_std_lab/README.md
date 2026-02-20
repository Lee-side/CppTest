# cpp_std_lab

这个 Demo 是一个可扩展的 C++ 标准实验台（第一版），用于比较不同标准下库函数的可用性与结果一致性。

当前只实现一个子命令：`nth_element`。

## 目标

- 统一入口：`<binary> <algo> [options]`
- 同一份源码同时构建 `C++17/20/23`
- 优先使用 `std::ranges::nth_element`，不可用时自动回退 `std::nth_element`
- 使用 `CTest` 校验结果，保证可重复验证

## 构建

```bash
cd demos/cpp_std_lab
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## 运行

默认样例（`nums=3,1,7,5,2`，`k=2`）：

```bash
./build/cpp_std_lab_cpp17 nth_element
./build/cpp_std_lab_cpp20 nth_element
./build/cpp_std_lab_cpp23 nth_element
```

自定义输入：

```bash
./build/cpp_std_lab_cpp20 nth_element --nums 9,1,4,4,8 --k 3
```

参数说明：

- `--nums`：逗号分隔整数列表
- `--k`：第 `k` 大（1-based，语义对应 `nums.end() - k`）

## 输出格式

程序输出固定为单行键值对：

```text
STD=<17|20|23>;ALGO=nth_element;IMPL=<ranges|fallback>;K=<k>;VALUE=<v>;N=<n>;OK=1
```

字段说明：

- `STD`：当前可执行文件对应的 C++ 标准
- `ALGO`：当前执行的算法子命令
- `IMPL`：实际调用实现（`ranges` 或 `fallback`）
- `VALUE`：第 `k` 大元素
- `N`：输入数组长度

## 测试

```bash
ctest --test-dir build -V
```

覆盖用例：

- `cpp17_default`
- `cpp20_default`
- `cpp23_default`
- `cpp20_custom_dup`
- `cpp20_custom_neg`
- `cpp20_invalid_k`
- `unknown_algo`

## 扩展新算法（最小步骤）

1. 在 `src/main.cpp` 增加新子命令分支（例如 `sort`）。
2. 复用参数解析与统一输出格式，添加 `ALGO=<name>`。
3. 在 `CMakeLists.txt` 追加对应 `CTest` 用例。
4. 在本 README 追加该子命令示例。
