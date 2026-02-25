# signal_cpp

这个 Demo 用 `std::signal` 做最小可运行实验，帮助你直观看到：

- 当前系统上常见信号是否能被 `signal` 注册处理器。
- 哪些信号天然不可捕获（通常 `SIGKILL`、`SIGSTOP`）。
- 收到信号后，程序是否进入自定义处理逻辑。

## 构建

```bash
cd demos/signal_cpp
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## 运行

```bash
./build/signal_demo
```

程序启动后会打印：

- `PID=<pid>`
- 一张注册结果表（`OK/FAIL` + 失败原因）
- 可直接复制的 `kill` 命令

## 体验命令

在另一个终端发送信号（把 `<pid>` 替换成程序打印出的 PID）：

```bash
kill -USR1 <pid>
kill -USR2 <pid>
kill -TERM <pid>   # 程序优雅退出
kill -KILL <pid>   # 无法捕获，进程会被直接杀死
kill -STOP <pid>   # 无法捕获，进程会暂停
kill -CONT <pid>   # 继续执行（若之前 STOP 过）
```

也可以直接在前台按 `Ctrl+C`（`SIGINT`）。

## 说明

- 这是 `signal()` 的教学 Demo，不是生产级信号处理框架。
- 实际工程建议优先使用 `sigaction`（语义更稳定、能力更完整）。
- 标准信号通常不排队；短时间重复发送同一种信号，可能会被合并。
