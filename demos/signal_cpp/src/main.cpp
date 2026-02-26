#include <cerrno>
#include <csignal>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include <unistd.h>

namespace {

#if defined(NSIG)
// NSIG 是系统可识别的信号编号上界（通常比最大信号值大 1）。
constexpr int kSignalSlots = NSIG;
#else
constexpr int kSignalSlots = 64;
#endif

struct SignalSpec {
  int sig;
  const char* name;
  // 收到该信号后是否请求主循环退出（用于演示优雅退出）。
  bool request_exit;
};

struct RegisterResult {
  int sig;
  const char* name;
  bool ok;
  int err;
};

volatile std::sig_atomic_t g_pending[kSignalSlots] = {};
volatile std::sig_atomic_t g_exit_requested = 0;

void SignalHandler(int sig) {
  // handler 中只做“最小动作”：记录信号编号，避免做复杂/不安全操作。
  if (sig > 0 && sig < kSignalSlots) {
    g_pending[sig] = 1;
  }

  // 这些信号到来后，主循环会在下一轮退出。
  switch (sig) {
#ifdef SIGINT
    case SIGINT:
#endif
#ifdef SIGTERM
    case SIGTERM:
#endif
#ifdef SIGQUIT
    case SIGQUIT:
#endif
      g_exit_requested = 1;
      break;
    default:
      break;
  }
}

void AddSignal(std::vector<SignalSpec>& specs, int sig, const char* name, bool request_exit = false) {
  // 防重复添加，避免同一信号被注册多次。
  for (const auto& spec : specs) {
    if (spec.sig == sig) {
      return;
    }
  }
  specs.push_back({sig, name, request_exit});
}

/*
 * 这个 Demo 按“应用常见信号场景”组织信号集合：
 * 1) 生命周期与运维控制：SIGINT/SIGTERM/SIGQUIT/SIGHUP
 *    - 常用于优雅退出、重载配置、人工中断。
 * 2) 业务自定义控制：SIGUSR1/SIGUSR2
 *    - 常用于打印运行状态、触发轻量运维动作。
 * 3) 子进程与管道：SIGCHLD/SIGPIPE
 *    - 前者用于回收子进程，后者表示向已关闭管道写数据。
 * 4) 致命错误类：SIGSEGV/SIGBUS/SIGILL/SIGFPE/SIGABRT
 *    - 实际工程里通常只做最小记录，然后尽快退出。
 * 5) 不可捕获类：SIGKILL/SIGSTOP
 *    - 这里故意尝试注册，便于演示“注册失败”。
 *
 * 如何测试（另一个终端）：
 *   kill -TERM <pid>  # 模拟服务停止
 *   kill -HUP  <pid>  # 模拟重载配置
 *   kill -USR1 <pid>  # 模拟自定义控制
 *   kill -KILL <pid>  # 演示不可捕获
 */
std::vector<SignalSpec> BuildSignalSpecs() {
  std::vector<SignalSpec> specs;
  // 这里列的是“常见信号集合”，是否存在取决于当前平台头文件定义。

  // 生命周期/会话类信号。
#ifdef SIGHUP
  AddSignal(specs, SIGHUP, "SIGHUP");
#endif
#ifdef SIGINT
  AddSignal(specs, SIGINT, "SIGINT", true);
#endif
#ifdef SIGQUIT
  AddSignal(specs, SIGQUIT, "SIGQUIT", true);
#endif

  // 致命错误与调试相关信号。
#ifdef SIGILL
  AddSignal(specs, SIGILL, "SIGILL");
#endif
#ifdef SIGTRAP
  AddSignal(specs, SIGTRAP, "SIGTRAP");
#endif
#ifdef SIGABRT
  AddSignal(specs, SIGABRT, "SIGABRT");
#endif
#ifdef SIGBUS
  AddSignal(specs, SIGBUS, "SIGBUS");
#endif
#ifdef SIGFPE
  AddSignal(specs, SIGFPE, "SIGFPE");
#endif
#ifdef SIGKILL
  // SIGKILL/SIGSTOP 理论上不可捕获，这里保留用于演示注册失败行为。
  AddSignal(specs, SIGKILL, "SIGKILL");
#endif

  // 业务自定义信号。
#ifdef SIGUSR1
  AddSignal(specs, SIGUSR1, "SIGUSR1");
#endif
#ifdef SIGSEGV
  AddSignal(specs, SIGSEGV, "SIGSEGV");
#endif
#ifdef SIGUSR2
  AddSignal(specs, SIGUSR2, "SIGUSR2");
#endif
#ifdef SIGPIPE
  AddSignal(specs, SIGPIPE, "SIGPIPE");
#endif
#ifdef SIGALRM
  AddSignal(specs, SIGALRM, "SIGALRM");
#endif

  // 常见服务管理与子进程信号。
#ifdef SIGTERM
  AddSignal(specs, SIGTERM, "SIGTERM", true);
#endif
#ifdef SIGCHLD
  AddSignal(specs, SIGCHLD, "SIGCHLD");
#endif
#ifdef SIGCONT
  AddSignal(specs, SIGCONT, "SIGCONT");
#endif
#ifdef SIGSTOP
  AddSignal(specs, SIGSTOP, "SIGSTOP");
#endif

  // 终端作业控制/资源限制/异步事件类。
#ifdef SIGTSTP
  AddSignal(specs, SIGTSTP, "SIGTSTP");
#endif
#ifdef SIGTTIN
  AddSignal(specs, SIGTTIN, "SIGTTIN");
#endif
#ifdef SIGTTOU
  AddSignal(specs, SIGTTOU, "SIGTTOU");
#endif
#ifdef SIGURG
  AddSignal(specs, SIGURG, "SIGURG");
#endif
#ifdef SIGXCPU
  AddSignal(specs, SIGXCPU, "SIGXCPU");
#endif
#ifdef SIGXFSZ
  AddSignal(specs, SIGXFSZ, "SIGXFSZ");
#endif
#ifdef SIGVTALRM
  AddSignal(specs, SIGVTALRM, "SIGVTALRM");
#endif
#ifdef SIGPROF
  AddSignal(specs, SIGPROF, "SIGPROF");
#endif
#ifdef SIGWINCH
  AddSignal(specs, SIGWINCH, "SIGWINCH");
#endif
#ifdef SIGIO
  AddSignal(specs, SIGIO, "SIGIO");
#endif
#ifdef SIGSYS
  AddSignal(specs, SIGSYS, "SIGSYS");
#endif

  return specs;
}

const char* SignalNameByNumber(int sig, const std::vector<SignalSpec>& specs) {
  for (const auto& spec : specs) {
    if (spec.sig == sig) {
      return spec.name;
    }
  }
  return "UNKNOWN";
}

void PrintRegistrationTable(const std::vector<RegisterResult>& results) {
  std::cout << std::left << std::setw(10) << "NAME"
            << std::setw(6) << "NUM"
            << std::setw(12) << "REGISTER"
            << "DETAIL" << '\n';
  std::cout << "-----------------------------------------------" << '\n';

  for (const auto& result : results) {
    std::string detail = result.ok ? "handler installed" : std::string(std::strerror(result.err));
    std::cout << std::left << std::setw(10) << result.name
              << std::setw(6) << result.sig
              << std::setw(12) << (result.ok ? "OK" : "FAIL")
              << detail << '\n';
  }
}

}  // namespace

int main() {
  const auto specs = BuildSignalSpecs();
  std::vector<RegisterResult> results;
  results.reserve(specs.size());

  // 逐个调用 std::signal，并把成功/失败原因打印出来。
  for (const auto& spec : specs) {
    errno = 0;
    const auto previous = std::signal(spec.sig, SignalHandler);
    if (previous == SIG_ERR) {
      results.push_back({spec.sig, spec.name, false, errno});
    } else {
      results.push_back({spec.sig, spec.name, true, 0});
    }
  }

  std::cout << "PID=" << ::getpid() << '\n';
  std::cout << "Using std::signal() to register handlers for common signals.\n";
  PrintRegistrationTable(results);

  std::cout << "\nQuick try (another terminal):\n";
#ifdef SIGUSR1
  std::cout << "  kill -USR1 " << ::getpid() << '\n';
#endif
#ifdef SIGUSR2
  std::cout << "  kill -USR2 " << ::getpid() << '\n';
#endif
#ifdef SIGTERM
  std::cout << "  kill -TERM " << ::getpid() << "    # graceful exit\n";
#endif
#ifdef SIGKILL
  std::cout << "  kill -KILL " << ::getpid() << "    # cannot be caught\n";
#endif
#ifdef SIGSTOP
  std::cout << "  kill -STOP " << ::getpid() << "    # cannot be caught\n";
#endif

  std::cout << "Press Ctrl+C to exit, or send SIGTERM/SIGQUIT.\n\n";

  std::vector<int> received_count(kSignalSlots, 0);

  while (!g_exit_requested) {
    // pause 阻塞等待任意信号到来，避免 busy-loop 占 CPU。
    ::pause();

    // 真正的输出与计数在主线程做，避免在 handler 中做 I/O。
    for (const auto& result : results) {
      if (!result.ok) {
        continue;
      }
      if (result.sig <= 0 || result.sig >= kSignalSlots) {
        continue;
      }
      if (!g_pending[result.sig]) {
        continue;
      }

      g_pending[result.sig] = 0;
      received_count[result.sig] += 1;

      const char* text = ::strsignal(result.sig);
      if (text == nullptr) {
        text = "unknown";
      }

      std::cout << "[caught] " << SignalNameByNumber(result.sig, specs)
                << "(" << result.sig << ")"
                << ", desc=\"" << text << "\""
                << ", count=" << received_count[result.sig] << '\n';
    }
  }

  std::cout << "Exit requested. bye.\n";
  return 0;
}
