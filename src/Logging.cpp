#include <crow/Logging.hpp>

#include <ctime>
#include <thread>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <unordered_map>

namespace crow {

    void _InternalLogger::Log(std::ostream& stream, const std::string& preamble, const std::string& type, std::string_view str) {
        auto thread_id = std::hash<std::thread::id>()(std::this_thread::get_id());
        
        static std::unordered_map<size_t, size_t> internal_thread_id;

        lock.lock();
        if (internal_thread_id.find(thread_id) == internal_thread_id.end()) {
            auto new_id = internal_thread_id.size();
            internal_thread_id[thread_id] = new_id;
        }

        thread_id = internal_thread_id[thread_id];
        lock.unlock();

        auto t = std::time(nullptr);
        auto now = std::localtime(&t);

        std::stringstream ss;

        static const char* days[] = {
            "Sun",
            "Mon",
            "Tue",
            "Wed",
            "Thu",
            "Fri",
            "Sat"
        };

        static const char* months[] = {
            "Jan",
            "Feb",
            "Mar",
            "Apr",
            "May",
            "Jun",
            "Jul",
            "Aug",
            "Sep",
            "Oct",
            "Nov",
            "Dec"
        };

        ss << "[" << thread_id << "][";
        ss << days[now->tm_wday] << " " << months[now->tm_mon] << " " << now->tm_mday << ", " << (now->tm_year + 1900) << " "
           << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec;
        ss << "]" << type << str;

        lock.lock();
        if (file.is_open()) {
            file << ss.str() << "\n";
        }

        stream << preamble << ss.str() << "\033[0m";
        stream << std::endl;

        lock.unlock();
    }

    void _InternalLogger::Info(std::string_view str) {
        Log(std::cout, "", "[Info]", str);
    }

    void _InternalLogger::Warning(std::string_view str) {
        Log(std::cout, "\033[33m", "[Warning]", str);
    }

    void _InternalLogger::Error(std::string_view str) {
        Log(std::cerr, "\033[31m", "[Error]", str);
    }

    void _InternalLogger::Critical(std::string_view str) {
        Log(std::cerr, "\033[30;41m", "[Critical]", str);
        std::exit(EXIT_FAILURE);
    }

    void _InternalLogger::SetLogFile(std::string_view path) {
        lock.lock();

        file = std::ofstream(std::string(path));

        lock.unlock();

        if (!file.is_open())
            engine::Error("Could not open {} for writing of the log file", path);
    }

    _InternalLogger _internal_logger;

}