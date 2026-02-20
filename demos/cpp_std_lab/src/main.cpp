#include <algorithm>
#include <charconv>
#include <cstddef>
#include <iostream>
#include <string>
#include <system_error>
#include <vector>

#ifdef __has_include
#if __has_include(<version>)
#include <version>
#endif
#if __has_include(<ranges>)
#include <ranges>
#endif
#endif

#ifndef DEMO_STD
#define DEMO_STD 0
#endif

#if defined(__cplusplus) && (__cplusplus >= 202002L) && defined(__cpp_lib_ranges) && (__cpp_lib_ranges >= 201911L)
#define CPP_STD_LAB_HAS_RANGES 1
#else
#define CPP_STD_LAB_HAS_RANGES 0
#endif

namespace {

struct Config {
  std::vector<int> nums{3, 1, 7, 5, 2};
  std::size_t k{2};
};

struct Result {
  int kth_value{0};
  std::string impl;
};

int fail(const std::string& message) {
  std::cerr << "error: " << message << '\n';
  return 2;
}

bool parse_int(const std::string& text, int& value) {
  const char* begin = text.data();
  const char* end = text.data() + text.size();
  const auto [ptr, ec] = std::from_chars(begin, end, value);
  return ec == std::errc() && ptr == end;
}

bool parse_positive_size(const std::string& text, std::size_t& value) {
  unsigned long long parsed = 0;
  const char* begin = text.data();
  const char* end = text.data() + text.size();
  const auto [ptr, ec] = std::from_chars(begin, end, parsed);
  if (ec != std::errc() || ptr != end) {
    return false;
  }
  value = static_cast<std::size_t>(parsed);
  return true;
}

bool parse_csv_ints(const std::string& text, std::vector<int>& out) {
  if (text.empty()) {
    return false;
  }

  out.clear();
  std::size_t start = 0;
  while (start <= text.size()) {
    const std::size_t comma = text.find(',', start);
    const std::size_t len = (comma == std::string::npos) ? text.size() - start : comma - start;
    if (len == 0) {
      return false;
    }

    int value = 0;
    if (!parse_int(text.substr(start, len), value)) {
      return false;
    }
    out.push_back(value);

    if (comma == std::string::npos) {
      break;
    }
    start = comma + 1;
  }

  return !out.empty();
}

bool parse_cli(int argc, char** argv, std::string& algo, Config& cfg, std::string& error) {
  if (argc < 2) {
    error = "missing algorithm, usage: <binary> <algo> [--nums a,b,c] [--k n]";
    return false;
  }

  algo = argv[1];

  for (int i = 2; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--nums") {
      if (i + 1 >= argc) {
        error = "--nums requires a value";
        return false;
      }
      if (!parse_csv_ints(argv[++i], cfg.nums)) {
        error = "invalid --nums, expected comma-separated integers";
        return false;
      }
      continue;
    }

    if (arg == "--k") {
      if (i + 1 >= argc) {
        error = "--k requires a value";
        return false;
      }
      if (!parse_positive_size(argv[++i], cfg.k)) {
        error = "invalid --k, expected a positive integer";
        return false;
      }
      continue;
    }

    error = "unknown option: " + arg;
    return false;
  }

  return true;
}

Result run_nth_element(const Config& cfg) {
  std::vector<int> data = cfg.nums;
  const auto nth = data.end() - static_cast<std::ptrdiff_t>(cfg.k);

#if CPP_STD_LAB_HAS_RANGES
  std::ranges::nth_element(data, nth);
  return Result{*nth, "ranges"};
#else
  std::nth_element(data.begin(), nth, data.end());
  return Result{*nth, "fallback"};
#endif
}

}  // namespace

int main(int argc, char** argv) {
  std::string algo;
  std::string error;
  Config cfg;

  if (!parse_cli(argc, argv, algo, cfg, error)) {
    return fail(error);
  }

  if (algo != "nth_element") {
    return fail("unsupported algorithm: " + algo);
  }

  if (cfg.nums.empty()) {
    return fail("nums cannot be empty");
  }

  if (cfg.k == 0 || cfg.k > cfg.nums.size()) {
    return fail("k must be in [1, nums.size()]");
  }

  const Result result = run_nth_element(cfg);
  std::cout << "STD=" << DEMO_STD << ";ALGO=nth_element;IMPL=" << result.impl << ";K=" << cfg.k
            << ";VALUE=" << result.kth_value << ";N=" << cfg.nums.size() << ";OK=1\n";
  return 0;
}
