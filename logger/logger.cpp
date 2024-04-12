#include <cassert>
#include <format>
#include <iostream>
#include <optional>

struct Logger {
  enum class LogLevel { Info, Debug, Warning, Error, Critical, Disabled };

  Logger(LogLevel level = LogLevel::Info, std::ostream& out_stream = std::cout,
         std::ostream& err_stream = std::cerr)
      : m_out_stream{out_stream}, m_err_stream{err_stream}, m_level{level} {}

  template <typename... TFmtArgs>
  void log_info(std::format_string<TFmtArgs...> fmt_str,
                TFmtArgs&&... fmt_args) {
    log<LogLevel::Info>(
        m_out_stream,
        std::format(fmt_str, std::forward<TFmtArgs>(fmt_args)...));
  }
  template <typename... TFmtArgs>
  void log_warning(std::format_string<TFmtArgs...> fmt_str,
                   TFmtArgs&&... fmt_args) {
    log<LogLevel::Warning>(
        m_out_stream,
        std::format(fmt_str, std::forward<TFmtArgs>(fmt_args)...));
  }
  template <typename... TFmtArgs>
  void log_error(std::format_string<TFmtArgs...> fmt_str,
                 TFmtArgs&&... fmt_args) {
    log<LogLevel::Error>(
        m_err_stream,
        std::format(fmt_str, std::forward<TFmtArgs>(fmt_args)...));
  }
  template <typename... TFmtArgs>
  void log_debug(std::format_string<TFmtArgs...> fmt_str,
                 TFmtArgs&&... fmt_args) {
    log<LogLevel::Debug>(
        m_err_stream,
        std::format(fmt_str, std::forward<TFmtArgs>(fmt_args)...));
  }
  template <typename... TFmtArgs>
  void log_critical(std::format_string<TFmtArgs...> fmt_str,
                    TFmtArgs&&... fmt_args) {
    log<LogLevel::Critical>(
        m_err_stream,
        std::format(fmt_str, std::forward<TFmtArgs>(fmt_args)...));
  }

  void flush() {
    m_out_stream << std::flush;
    m_err_stream << std::flush;
  }

private:
  template <LogLevel TLogLevel>
  void log(std::ostream& os, const std::string& msg) {
    if (TLogLevel <= m_level) {
      os << log_level_str<TLogLevel>() << " " << msg << std::endl;
    }
  }

  template <LogLevel TLogLevel>
  static constexpr const char* log_level_str() {
    static_assert(TLogLevel <= LogLevel::Disabled);
    if constexpr (TLogLevel == LogLevel::Info) {
      return "[Info]";
    }
    if constexpr (TLogLevel == LogLevel::Debug) {
      return "[Debug]";
    }
    if constexpr (TLogLevel == LogLevel::Warning) {
      return "[Warning]";
    }
    if constexpr (TLogLevel == LogLevel::Error) {
      return "[Error]";
    }
    if constexpr (TLogLevel == LogLevel::Critical) {
      return "[Critical]";
    }
    if constexpr (TLogLevel == LogLevel::Disabled) {
      return "[Disabled]";
    }
  }

  std::ostream& m_out_stream;
  std::ostream& m_err_stream;

  LogLevel m_level;
};

namespace global {
Logger& get_logger(
    std::optional<std::tuple<Logger::LogLevel, std::ostream&, std::ostream&>>
        params = std::nullopt) {
  static Logger logger = [](auto& inner_params) -> Logger {
    if (inner_params.has_value()) {
      return std::apply([](auto&... args) { return Logger(args...); },
                        inner_params.value());
      return Logger();
    } else {
      return Logger();
    }
  }(params);

  return logger;
}
}  // namespace global

auto main() -> int {
  auto& logger = global::get_logger();

  logger.log_debug("{}", "Hello, World!");

  assert(&global::get_logger() == &global::get_logger());
}