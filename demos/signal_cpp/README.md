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

## 应用里通常建议关注的信号

下面这张表偏“应用实践”，不是 POSIX 全量清单。是否支持、编号是多少，取决于平台。

| 信号 | 常见含义 | App 常见处理动作 | 触发方式（测试） |
| --- | --- | --- | --- |
| `SIGINT` | 用户中断（前台 `Ctrl+C`） | 开始优雅退出（停止接收请求、清理资源） | 前台 `Ctrl+C` 或 `kill -INT <pid>` |
| `SIGTERM` | 外部请求终止（服务停止） | 优雅退出，通常这是首选停止信号 | `kill -TERM <pid>` |
| `SIGQUIT` | 退出请求（常用于调试） | 退出；有些系统可能伴随调试信息 | `kill -QUIT <pid>` 或前台 `Ctrl+\` |
| `SIGHUP` | 会话挂断/重载配置约定 | 常用于“重载配置不重启” | `kill -HUP <pid>` |
| `SIGUSR1` / `SIGUSR2` | 用户自定义信号 | 打印状态、切日志级别等自定义动作 | `kill -USR1 <pid>` / `kill -USR2 <pid>` |
| `SIGCHLD` | 子进程状态变化 | `waitpid` 回收子进程，避免僵尸进程 | `kill -CHLD <pid>`（演示） |
| `SIGPIPE` | 向已关闭管道/套接字写数据 | 忽略或转成错误处理，避免进程直接终止 | `kill -PIPE <pid>`（演示） |
| `SIGALRM` | 定时器到期 | 超时处理、定时任务触发 | `kill -ALRM <pid>` 或代码里 `alarm()` |
| `SIGTSTP` / `SIGCONT` | 进程暂停/继续（作业控制） | 暂停耗时任务、恢复任务 | `kill -TSTP <pid>` / `kill -CONT <pid>` |
| `SIGSEGV` / `SIGBUS` / `SIGILL` / `SIGFPE` / `SIGABRT` | 致命错误 | 最小记录后尽快退出；不要尝试恢复业务 | `kill -SEGV <pid>` 等（仅演示） |
| `SIGKILL` / `SIGSTOP` | 强制结束/强制暂停 | 无法捕获、无法忽略、无法自定义处理 | `kill -KILL <pid>` / `kill -STOP <pid>` |

## 如何系统化触发测试

先启动程序并拿到 PID：

```bash
./build/signal_demo
```

再在另一个终端执行（把 `<pid>` 改成实际 PID）：

```bash
# 1) 常见“可处理”信号
kill -HUP <pid>
kill -USR1 <pid>
kill -USR2 <pid>
kill -PIPE <pid>

# 2) 暂停/恢复
kill -TSTP <pid>
sleep 2
kill -CONT <pid>

# 3) 优雅退出（最后执行）
kill -TERM <pid>
```

如果你想做一轮批量测试：

```bash
PID=<pid>
for s in HUP USR1 USR2 PIPE; do
  kill -"$s" "$PID"
  sleep 1
done
kill -TERM "$PID"
```

## 崩溃采集方案讨论（Linux/Android 精简版）

### core dump 与信号

- core dump 通常由致命信号触发，常见：`SIGSEGV`、`SIGABRT`、`SIGBUS`、`SIGILL`、`SIGFPE`。
- `SIGTERM`、`SIGINT`、`SIGKILL` 通常不产生 core。
- 是否真正生成 core 文件还取决于系统配置（如 `ulimit -c`、系统 core 策略）。

### Breakpad / Crashpad 的定位

- 两者核心都是“崩溃采集 + dump 生成 + 符号化分析”，不是“优雅退出框架”。
- Breakpad 历史更久；Crashpad 是后续方案，通常更适合新项目优先评估。
- 在 Linux/Android，接入成本通常可控；长期成本主要在符号文件管理、上报链路、解析平台运维。

### Crashpad 的进程模型与开销

- Crashpad 常见是独立 `handler` 进程。
- 常驻模式：平时多一个进程，稳定性更高。
- 崩溃拉起模式：平时不常驻，崩溃时拉起处理后退出。
- 评估开销建议关注业务实测的 `RSS/CPU/IO` 峰值，而不是只看单点经验值。

### 其他可选方案

- 自建偏底层：`Crashpad` / `Breakpad` / Linux `systemd-coredump`。
- 平台化托管：`Sentry Native`、`Bugsnag`、`Firebase Crashlytics (NDK, Android)`。
- 选型优先看：平台覆盖、符号化能力、隐私合规、运维投入，不要只看 GitHub star。

## 注意事项（工程实践）

- `SIGKILL`、`SIGSTOP` 永远无法被应用捕获，这是内核保证的。
- 致命错误类信号通常表示进程状态已损坏，生产环境不建议“继续跑”。
- `signal()` 适合教学和轻量实验；正式项目建议使用 `sigaction`。
- 各平台信号编号可能不同，本 Demo 打印出的 `NUM` 以当前机器结果为准。

## 说明

- 这是 `signal()` 的教学 Demo，不是生产级信号处理框架。
- 实际工程建议优先使用 `sigaction`（语义更稳定、能力更完整）。
- 标准信号通常不排队；短时间重复发送同一种信号，可能会被合并。
