#ifndef CROW_READ_WRITE_LOCK_HPP
#define CROW_READ_WRITE_LOCK_HPP

#include <atomic>
#include <mutex>

#include "Crow.hpp"

namespace crow {

    /// @brief This is like a mutex, but allows multiple readers
    class API ReadWriteLock {
    private:
        /// @brief The number of readers currently reading
        std::atomic<uint32_t> readers = 0;

        /// @brief This is locked when there is a writer
        std::mutex lock;

    public:
        /// @brief This does nothing, just a constructor
        ReadWriteLock() = default;

        /// @brief Dont allow copy
        ReadWriteLock(const ReadWriteLock&) = delete;

        /// @brief Dont allow copy
        ReadWriteLock& operator=(const ReadWriteLock&) = delete;

        /// @brief Dont allow move
        ReadWriteLock(ReadWriteLock&&) = delete;

        /// @brief Dont allow move
        ReadWriteLock& operator=(ReadWriteLock&&) = delete;

        /// @brief If there is a writer, wait for it to unlock this. Then
        /// increments the readers
        inline void LockReading() {
            lock.lock();
            readers++;
            lock.unlock();
        }

        /// @brief Decrements the readers
        inline void UnlockReading() { readers--; }

        /// @brief Lock for writing. Then wait until there are no readers
        inline void LockWriting() {
            while (true) {
                lock.lock();
                if (readers == 0) break;
                lock.unlock();
            }
        }

        /// @brief Unlock for reading
        inline void UnlockWriting() { lock.unlock(); }
    };

};

#endif