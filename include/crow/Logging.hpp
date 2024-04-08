#ifndef CROW_LOGGING_HPP
#define CROW_LOGGING_HPP

#include <string>
#include <string_view>
#include <fstream>
#include <format>
#include <mutex>

#include "Crow.hpp"

namespace crow {

    class API _InternalLogger {
    private:
        std::mutex lock;
        std::ofstream file;

        void Log(std::ostream& stream, const std::string& preamble, const std::string& type, std::string_view str);

    public:
        void Info(std::string_view str);
        void Warning(std::string_view str);
        void Error(std::string_view str);
        void Critical(std::string_view str);
        
        void SetLogFile(std::string_view path);
    };

    extern _InternalLogger API _internal_logger;

    namespace app {
        
        template <class... Args>
        inline void Info(const std::format_string<Args...> fmt, Args&&... args) {
            auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            str = std::string("[App] ") + str;
            _internal_logger.Info(str);
        }

        template <class... Args>
        inline void Warning(const std::format_string<Args...> fmt, Args&&... args) {
            auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            str = std::string("[App] ") + str;
            _internal_logger.Warning(str);
        }

        template <class... Args>
        inline void Error(const std::format_string<Args...> fmt, Args&&... args) {
            auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            str = std::string("[App] ") + str;
            _internal_logger.Error(str);
        }

        template <class... Args>
        inline void Critical(const std::format_string<Args...> fmt, Args&&... args) {
            auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            str = std::string("[App] ") + str;
            _internal_logger.Critical(str);
        }

    }

    namespace engine {

        template <class... Args>
        inline void Info(const std::format_string<Args...> fmt, Args&&... args) {
            auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            str = std::string("[Engine] ") + str;
            _internal_logger.Info(str);
        }

        template <class... Args>
        inline void Warning(const std::format_string<Args...> fmt, Args&&... args) {
            auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            str = std::string("[Engine] ") + str;
            _internal_logger.Warning(str);
        }

        template <class... Args>
        inline void Error(const std::format_string<Args...> fmt, Args&&... args) {
            auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            str = std::string("[Engine] ") + str;
            _internal_logger.Error(str);
        }

        template <class... Args>
        inline void Critical(const std::format_string<Args...> fmt, Args&&... args) {
            auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            str = std::string("[Engine] ") + str;
            _internal_logger.Critical(str);
        }

    }

    inline void SetLoggingFile(std::string_view path) {
        _internal_logger.SetLogFile(path);
    }
}

#endif