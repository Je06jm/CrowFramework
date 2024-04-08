#include <crow/Actor.hpp>

#ifdef WINDOWS
#include <Windows.h>
#else
#include <sched.h>
#endif

namespace crow {

    ActorScheduler::ActorScheduler(size_t thread_count) {
        for (size_t i = 0; i < thread_count - 1; i++) {
            threads.emplace_back(std::thread([&]() {
                while (running) {
                    if (!ProcessMessage(false)) YieldCPU();
                }
            }));
        }
    }

    ActorScheduler::~ActorScheduler() {
        running = false;

        for (auto& thread : threads) thread.join();
    }

    void ActorScheduler::YieldCPU() const {
#ifdef WINDOWS
        YieldProcessor();
#else
        sched_yield();
#endif
    }

    bool ActorScheduler::ProcessMessage(bool is_main) {
        ActorPtr actor = nullptr;

        lock.lock();
        if (is_main) {
            if (main_to_do.size() != 0) {
                actor = main_to_do.front();
                main_to_do.erase(main_to_do.begin());
            }
            else if (to_do.size() != 0) {
                actor = to_do.front();
                to_do.erase(to_do.begin());
            }
        }
        else if (to_do.size() != 0) {
            actor = to_do.front();
            to_do.erase(to_do.begin());
        }
        lock.unlock();

        if (!actor) return false;

        working++;
        actor->ProcessMessage();
        working--;

        return true;
    }

    void ActorScheduler::ProcessAllMessages() {
        while (true) {
            if (ProcessMessage(true)) continue;

            if (working > 0) continue;

            break;
        }
    }

    std::unique_ptr<ActorScheduler> actor_scheduler = nullptr;

}