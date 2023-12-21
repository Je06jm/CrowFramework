#ifndef CROW_READ_WRITE_LOCK_HPP
#define CROW_READ_WRITE_LOCK_HPP

#include <atomic>
#include <mutex>

#include "Crow.hpp"

namespace crow {

    class API ReadWriteLock {
    private:
        std::atomic<uint32_t> readers = 0;
        std::mutex lock;

    public:
        ReadWriteLock() = default;

        ReadWriteLock(const ReadWriteLock&) = delete;
        ReadWriteLock& operator=(const ReadWriteLock&) = delete;

        ReadWriteLock(ReadWriteLock&&) = delete;
        ReadWriteLock& operator=(ReadWriteLock&&) = delete;

        inline void LockReading() {
            lock.lock();
            readers++;
            lock.unlock();
        }

        inline void UnlockReading() { readers--; }

        inline void LockWriting() {
            while (true) {
                lock.lock();
                if (readers == 0) break;
                lock.unlock();
            }
        }

        inline void UnlockWriting() { lock.unlock(); }
    };

};

#endif