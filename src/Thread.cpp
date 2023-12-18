#include <crow/Thread.hpp>

namespace crow {

    std::mutex Thread::next_free_id_lock;
    Thread::ID Thread::next_free_id = 0;
    std::unordered_map<std::thread::id, Thread::ID> Thread::thread_id_map;

}