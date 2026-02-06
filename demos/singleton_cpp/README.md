# singleton_cpp

这个 Demo 展示 3 种“安全可用”的单例实践：

1. 进程内 `static` 局部变量单例（Meyers Singleton）。
2. 堆区单例（`std::call_once` + `new`，线程安全初始化）。
3. 导出 `.so/.dylib` 时的单例安全做法：单例集中放到一个 owner 库里，其他库只通过导出接口访问。

## 目录说明

- `src/main.cpp`：运行入口，验证并打印三种场景结果。
- `src/so_singleton_owner.cpp`：进程级单例 owner，导出 `so_singleton_instance/so_singleton_next`。
- `src/consumer_one.cpp` / `src/consumer_two.cpp`：模拟两个业务 so，各自有“本地单例”，并调用 owner 导出接口。

## 构建

```bash
cd demos/singleton_cpp
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## 运行

```bash
./build/singleton_demo
```

## 预期结果

- `static local singleton` 和 `heap singleton + call_once` 最终计数都应等于 `线程数 * 循环次数`，说明并发访问没有丢失更新。
- `consumer_one` 和 `consumer_two` 的 `local singleton addr` 应不同（每个 so 各一份）。
- 两者的 `exported singleton addr` 应相同（通过 owner so 获取的是同一份进程级单例）。

## 导出 so 的单例实践要点

- 不要在多个业务 so 里各自暴露“同名/同语义”单例对象。
- 由一个专门的 owner so 持有单例实例并导出访问函数。
- 其余 so 通过接口访问 owner，避免多份实例和符号可见性导致的行为不一致。
