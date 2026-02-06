# dlopen_symbol_collision

这个 Demo 用于验证：

- App 先后加载两个共享库（`demo_one` / `demo_two`）。
- 两个库都导出同名全局变量和同名函数（`g_shared_value` / `shared_compute` / `shared_touch`）。
- 再通过每个 so 自己导出的接口读取，观察同名符号是否会“只生效一份”（符号抢占/插桩效果）。
- 同时对比同名符号与各自唯一符号（`g_unique_one` / `g_unique_two`）的行为。

## 构建

```bash
cd demos/dlopen_symbol_collision
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## 运行

在 build 目录运行，分别测试两种加载顺序：

```bash
cd build
./dlopen_symbol_demo 12
./dlopen_symbol_demo 21
```

- `12`：先加载 `demo_one`，再加载 `demo_two`
- `21`：先加载 `demo_two`，再加载 `demo_one`

## 观察点

重点看三类输出：

1. 每个库 `snapshot` 里“直接访问”同名变量/函数的值与地址（`shared *` 行）。
2. 每个库 `snapshot` 里通过 `dlsym(RTLD_DEFAULT, ...)` 解析同名符号的结果（`default *` 行）。
3. App 自己通过 `RTLD_DEFAULT` 解析到的同名变量/函数地址与返回值。

如果你看到两个库里同名符号的地址/值出现“跟随首个加载库”的现象，就说明发生了同名导出符号抢占；  
而唯一符号一般保持各库独立，可作为对照组。

## `dlsym` 是什么

`dlsym` 是运行时动态符号查找接口（定义在 `<dlfcn.h>`）：

- 你先用 `dlopen` 拿到库句柄；
- 再用 `dlsym(handle, "symbol_name")` 按名字取符号地址；
- 把返回的 `void*` 转成目标函数指针或变量指针后调用/读取。

本 Demo 里除了用具体库句柄查接口函数，还用了：

- `dlsym(RTLD_DEFAULT, "xxx")`：在“当前进程的全局可见符号集合”里按动态链接器的搜索顺序找第一个匹配项。  
  所以它非常适合拿来观察“同名符号最终被解析到谁”。

## Linux / ELF 差异（更强符号抢占）

在 Linux 的 ELF 体系里，默认符号可见性下通常会出现比 macOS 更强的 interposition（符号抢占）：

- 同名导出符号进入全局作用域后，后加载库的同名引用更容易被先加载库“截获”。
- 不仅 `RTLD_DEFAULT`，某些库内部对同名符号的引用也可能被抢占（取决于链接方式与重定位）。
- 常见现象是：第二个 so 的同名变量/函数访问结果“看起来像第一个 so 的实现”。

影响抢占强度的常见因素：

- 符号可见性（`default` vs `hidden`）
- 链接选项（如 `-Wl,-Bsymbolic` / `-Wl,-Bsymbolic-functions`）
- `dlopen` 标志（如 `RTLD_GLOBAL`、`RTLD_LOCAL`、`RTLD_DEEPBIND`）

建议在 Linux 下额外做 2 组实验：

1. 维持当前参数（`RTLD_GLOBAL`）跑 `12/21`，记录同名符号地址与返回值。  
2. 再尝试给 so 增加 `-Wl,-Bsymbolic` 或把导出改 `hidden`，对比抢占是否明显减弱。
