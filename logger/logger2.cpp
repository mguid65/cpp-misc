#include <fmt/chrono.h>
#include <fmt/format.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <source_location>
#include <string>
#include <string_view>
#include <vector>
#include <version>
#include <algorithm>

enum class LogLevel : std::uint8_t {
    debug,     // 0
    info,      // 1
    warning,   // 2
    error,     // 3
    critical,  // 4
    disabled   // 5
};

template <LogLevel Level>
[[nodiscard]] std::string_view log_level_string() {
    static_assert(Level < LogLevel::disabled);
    if constexpr (Level == LogLevel::debug) {
        return {"Debug"};
    }
    if constexpr (Level == LogLevel::info) {
        return {"Info"};
    }
    if constexpr (Level == LogLevel::warning) {
        return {"Warning"};
    }
    if constexpr (Level == LogLevel::error) {
        return {"Error"};
    }
    if constexpr (Level == LogLevel::critical) {
        return {"Critical"};
    }
}

struct DefaultFormatter {
    template <bool WithSrcLoc, typename... Args>
    [[nodiscard]] std::string format_log(Args&&... args) const {
        if constexpr (WithSrcLoc) {
            return fmt::format("{datetime}|{level}|{src_loc}|{msg}",
                               std::forward<Args>(args)...);
        } else {
            return fmt::format("{datetime}|{level}||{msg}",
                               std::forward<Args>(args)...);
        }
    }
};

struct StdoutLogSink {
    void log(std::string_view msg) { std::cout << msg << std::endl; }
};

template <typename MsgMask = decltype([](std::string_view msg) { return std::string(msg); })>
struct FilteringStdoutLogSink {
    void log(std::string_view msg) {
        constexpr static auto mask = MsgMask{};
        if (mask(msg) != mask(last_seen.first)) {
            if (last_seen.second != 0) {
                fmt::print(stdout, " ... repeated {} times\n", last_seen.second);
            } else if (!last_seen.first.empty()) {
                std::cout << std::endl;
            }
            last_seen.first = std::string(msg);
            last_seen.second = 0;
            std::cout << msg;
        } else {
            last_seen.second++;
        }
    }
    ~FilteringStdoutLogSink() {
        if (!last_seen.first.empty() && last_seen.second != 0) {
            std::cout << fmt::format(" ... repeated {} times", last_seen.second) << std::endl;
        }
    }
    std::pair<std::string, std::size_t> last_seen{};
};

struct LogSink {
    struct LogSinkConcept {
        virtual void log(std::string_view msg) = 0;
    };

    template <typename Concrete>
    struct LogSinkModel : public LogSinkConcept {
       private:
        Concrete m_sink;

       public:
        LogSinkModel(Concrete sink) : m_sink{sink} {}

        void log(std::string_view msg) { m_sink.log(msg); }
        void add_filter() {}
    };

    template <typename Concrete>
    LogSink(Concrete&& sink)
        : m_concept{std::make_shared<LogSinkModel<Concrete>>(
              std::forward<Concrete>(sink))} {}

    void log(std::string_view msg) { m_concept->log(msg); }

   private:
    std::shared_ptr<LogSinkConcept> m_concept{nullptr};
};

template <typename Formatter = DefaultFormatter, bool EnableSrcLocation = true>
struct Logger {
    struct LogCtx {
        LogCtx(Logger& logger, std::source_location src_loc)
            : m_logger{logger}, m_src_loc{src_loc} {}
        template <typename... Args>
        void debug(fmt::format_string<Args...> fmt_str, Args&&... args) {
            m_logger.debug(m_src_loc, fmt_str, std::forward<Args>(args)...);
        }
        template <typename... Args>
        void info(fmt::format_string<Args...> fmt_str, Args&&... args) {
            m_logger.info(m_src_loc, fmt_str, std::forward<Args>(args)...);
        }
        template <typename... Args>
        void warning(fmt::format_string<Args...> fmt_str, Args&&... args) {
            m_logger.warning(m_src_loc, fmt_str, std::forward<Args>(args)...);
        }
        template <typename... Args>
        void error(fmt::format_string<Args...> fmt_str, Args&&... args) {
            m_logger.error(m_src_loc, fmt_str, std::forward<Args>(args)...);
        }
        template <typename... Args>
        void critical(fmt::format_string<Args...> fmt_str, Args&&... args) {
            m_logger.critical(m_src_loc, fmt_str, std::forward<Args>(args)...);
        }

       private:
        Logger& m_logger;
        std::source_location m_src_loc;
    };

    template <typename... Args>
    void debug(std::source_location src_loc,
               fmt::format_string<Args...> fmt_str, Args&&... args) {
        const auto time = std::chrono::system_clock::now();
        log<LogLevel::debug>(m_formatter.template format_log<EnableSrcLocation>(
            fmt::arg("datetime", fmt::format("{:%Y%m%d-%X}", time)),
            fmt::arg("level", log_level_string<LogLevel::debug>()),
            fmt::arg("src_loc",
                     fmt::format("{}:{}:{}", src_loc.file_name(),
                                 src_loc.function_name(), src_loc.line())),
            fmt::arg("msg",
                     fmt::format(fmt_str, std::forward<Args>(args)...))));
    }
    template <typename... Args>
    void info(std::source_location src_loc, fmt::format_string<Args...> fmt_str,
              Args&&... args) {
        const auto time = std::chrono::system_clock::now();
        log<LogLevel::info>(m_formatter.template format_log<EnableSrcLocation>(
            fmt::arg("datetime", fmt::format("{:%Y%m%d-%X}", time)),
            fmt::arg("level", log_level_string<LogLevel::info>()),
            fmt::arg("src_loc",
                     fmt::format("{}:{}:{}", src_loc.file_name(),
                                 src_loc.function_name(), src_loc.line())),
            fmt::arg("msg",
                     fmt::format(fmt_str, std::forward<Args>(args)...))));
    }
    template <typename... Args>
    void warning(std::source_location src_loc,
                 fmt::format_string<Args...> fmt_str, Args&&... args) {
        const auto time = std::chrono::system_clock::now();
        log<LogLevel::warning>(
            m_formatter.template format_log<EnableSrcLocation>(
                fmt::arg("datetime", fmt::format("{:%Y%m%d-%X}", time)),
                fmt::arg("level", log_level_string<LogLevel::warning>()),
                fmt::arg("src_loc",
                         fmt::format("{}:{}:{}", src_loc.file_name(),
                                     src_loc.function_name(), src_loc.line())),
                fmt::arg("msg",
                         fmt::format(fmt_str, std::forward<Args>(args)...))));
    }
    template <typename... Args>
    void error(std::source_location src_loc,
               fmt::format_string<Args...> fmt_str, Args&&... args) {
        const auto time = std::chrono::system_clock::now();
        log<LogLevel::error>(m_formatter.template format_log<EnableSrcLocation>(
            fmt::arg("datetime", fmt::format("{:%Y%m%d-%X}", time)),
            fmt::arg("level", log_level_string<LogLevel::error>()),
            fmt::arg("src_loc",
                     fmt::format("{}:{}:{}", src_loc.file_name(),
                                 src_loc.function_name(), src_loc.line())),
            fmt::arg("msg",
                     fmt::format(fmt_str, std::forward<Args>(args)...))));
    }
    template <typename... Args>
    void critical(std::source_location src_loc,
                  fmt::format_string<Args...> fmt_str, Args&&... args) {
        const auto time = std::chrono::system_clock::now();
        log<LogLevel::critical>(
            m_formatter.template format_log<EnableSrcLocation>(
                fmt::arg("datetime", fmt::format("{:%Y%m%d-%X}", time)),
                fmt::arg("level", log_level_string<LogLevel::critical>()),
                fmt::arg("src_loc",
                         fmt::format("{}:{}:{}", src_loc.file_name(),
                                     src_loc.function_name(), src_loc.line())),
                fmt::arg("msg",
                         fmt::format(fmt_str, std::forward<Args>(args)...))));
    }

    template <typename... Args>
    void debug(fmt::format_string<Args...> fmt_str, Args&&... args) {
        const auto time = std::chrono::system_clock::now();
        log<LogLevel::debug>(m_formatter.template format_log<false>(
            fmt::arg("datetime", fmt::format("{:%Y%m%d-%X}", time)),
            fmt::arg("level", log_level_string<LogLevel::debug>()),
            fmt::arg("msg",
                     fmt::format(fmt_str, std::forward<Args>(args)...))));
    }
    template <typename... Args>
    void info(fmt::format_string<Args...> fmt_str, Args&&... args) {
        const auto time = std::chrono::system_clock::now();
        log<LogLevel::info>(m_formatter.template format_log<false>(
            fmt::arg("datetime", fmt::format("{:%Y%m%d-%X}", time)),
            fmt::arg("level", log_level_string<LogLevel::info>()),
            fmt::arg("msg",
                     fmt::format(fmt_str, std::forward<Args>(args)...))));
    }
    template <typename... Args>
    void warning(fmt::format_string<Args...> fmt_str, Args&&... args) {
        const auto time = std::chrono::system_clock::now();
        log<LogLevel::warning>(m_formatter.template format_log<false>(
            fmt::arg("datetime", fmt::format("{:%Y%m%d-%X}", time)),
            fmt::arg("level", log_level_string<LogLevel::warning>()),
            fmt::arg("msg",
                     fmt::format(fmt_str, std::forward<Args>(args)...))));
    }
    template <typename... Args>
    void error(fmt::format_string<Args...> fmt_str, Args&&... args) {
        const auto time = std::chrono::system_clock::now();
        log<LogLevel::error>(m_formatter.template format_log<false>(
            fmt::arg("datetime", fmt::format("{:%Y%m%d-%X}", time)),
            fmt::arg("level", log_level_string<LogLevel::error>()),
            fmt::arg("msg",
                     fmt::format(fmt_str, std::forward<Args>(args)...))));
    }
    template <typename... Args>
    void critical(fmt::format_string<Args...> fmt_str, Args&&... args) {
        const auto time = std::chrono::system_clock::now();
        log<LogLevel::critical>(m_formatter.template format_log<false>(
            fmt::arg("datetime", fmt::format("{:%Y%m%d-%X}", time)),
            fmt::arg("level", log_level_string<LogLevel::critical>()),
            fmt::arg("msg",
                     fmt::format(fmt_str, std::forward<Args>(args)...))));
    }

    LogCtx with_ctx(
        std::source_location src_loc = std::source_location::current()) {
        return LogCtx{*this, src_loc};
    }

   private:
    Formatter m_formatter;
    LogLevel m_level{LogLevel::debug};
    std::vector<std::pair<std::string, LogSink>> m_sinks{
        {"default", FilteringStdoutLogSink<
          decltype([](std::string_view msg) -> std::string {
            if (msg.empty()) { return {""}; }
            return std::string(msg.substr(msg.find('|')));
          })
        >{}}};

    template <LogLevel Level>
    [[nodiscard]] bool should_log() const {
        return m_level <= Level;
    }

    template <LogLevel Level>
    void log(std::string_view msg) {
        if (!should_log<Level>()) {
            return;
        }

        for (auto& sink : m_sinks) {
            sink.second.log(msg);
        }
    }
};

template <typename Formatter = DefaultFormatter, bool EnableSrcLocation = true>
Logger<Formatter, EnableSrcLocation> & get_logger(/*std::optional<LoggerOpts> opts = std::nullopt*/) {
    static Logger<Formatter, EnableSrcLocation> logger{/* opts.value_or(LoggerOpts{})*/};
    return logger;
}
