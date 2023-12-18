#ifndef CROW_THREAD_HPP
#define CROW_THREAD_HPP

#include "Attribute.hpp"

#include <thread>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace crow {

    class Thread : public AttributeHolder {
    public:
        using ID = uint32_t;

        static constexpr ID INVALID = -1U;

    private:
        ID id = INVALID;

        std::unique_ptr<std::thread> thread;

        static std::mutex next_free_id_lock;
        static ID next_free_id;
        static std::unordered_map<std::thread::id, ID> thread_id_map;

    protected:
        inline Thread() : thread{nullptr} {
            std::lock_guard<std::mutex> guard(next_free_id_lock);
            id = next_free_id++;
            thread_id_map[std::this_thread::get_id()] = id;
        }

        template <typename... Args>
        inline Thread(Args &&...args) : thread(new std::thread(args)) {
            std::lock_guard<std::mutex> guard(next_free_id_lock);
            id = next_free_id++;
            thread_id_map[std::this_thread::get_id()] = id;
        }

    public:
        virtual ~Thread() {}

        Thread(const Thread &) = delete;
        Thread &operator=(const Thread &) = delete;

        inline Thread(Thread &&thread) {
            id = thread.id;
            thread.id = INVALID;

            this->thread = std::move(thread.thread);
        }

        inline Thread &operator=(Thread &&thread) {
            id = thread.id;
            thread.id = INVALID;

            this->thread = std::move(thread.thread);
            return *this;
        }

        inline ID GetID() const {
            return id;
        }

        inline static ID GetCurrentThreadID() {
            std::lock_guard<std::mutex> guard(next_free_id_lock);

            auto thread_id = std::this_thread::get_id();

            if (thread_id_map.find(thread_id) != thread_id_map.end()) {
                return thread_id_map[thread_id];
            } else {
                return INVALID;
            }
        }
    };

    class SpawnedThread : public Thread {
    public:
        template <typename... Args>
        inline SpawnedThread(Args &&...args) : Thread(args) {}
    };

    class MainThread : public Thread {
    public:
        inline MainThread() : Thread() {
            AddAttribute(ATTRIBUTE_MAIN_THREAD);
        }

        template <typename Func, typename... Args>
        inline void Run(Func &&func, Args &&...args) {
            func(args);
        }
    };

    inline CROW_ATTRIBUTE(ATTRIBUTE_MAIN_THREAD);

}

#endif