#include <crow/Actor.hpp>

#ifdef WINDOWS
#include <Windows.h>
#else
#include <sched.h>
#endif

namespace crow {

    CROW_DEFINE_ATTRIBUTE(ATTRIBUTE_ACTOR_RENDERING);
    CROW_DEFINE_ATTRIBUTE(ATTRIBUTE_ACTOR_NON_CRITICAL);
    CROW_DEFINE_ATTRIBUTE(ATTRIBUTE_ACTOR_REGULAR);

    std::mutex ActorScheduler::schedulers_lock;
    std::unordered_map<Attribute, ActorScheduler*> ActorScheduler::schedulers;

    ActorScheduler::ActorScheduler(const Attribute& attribute, size_t thread_count) : attribute{attribute}, thread_count{thread_count} {
        gen = std::mt19937(rd());
        
        for (size_t i = 0; i < thread_count; i++) {
            threads.emplace_back([&]() {
                while (running) {
                    if (!ProcessOneMessage()) {
#ifdef WINDOWS
                        YieldProcessor();
#else
                        sched_yield();
#endif
                    }
                }
            });
        }
    }

    bool ActorScheduler::ProcessOneMessage() {
        managers_lock.LockReading();

        for (auto& manager : managers) {
            if (manager.second->ProcessOneMessage()) {
                managers_lock.UnlockReading();

                return true;
            }
        }

        managers_lock.UnlockReading();
        return false;
    }

    ActorScheduler::~ActorScheduler() {
        running = false;
        for (auto& thread : threads) {
            thread.join();
        }
    }

    void ActorScheduler::Run(bool until_empty) {
        while (running) {
            if (!ProcessOneMessage()) {
                if (until_empty) break;

#ifdef WINDOWS
                YieldProcessor();
#else
                sched_yield();
#endif
            }
        }
    }

    void ActorScheduler::BlockUntilEmpty() const {
        while (running) {
            managers_lock.LockReading();
            
            bool has_messages = false;
            for (auto& manager : managers) {
                if (manager.second->HasMessages()) {
                    has_messages = true;
                    break;
                }
            }

            managers_lock.UnlockReading();

            if (!has_messages) break;
        }
    }

    ActorScheduler* ActorScheduler::GetScheduler(const Attribute& attribute) {
        std::lock_guard<std::mutex> guard(schedulers_lock);
        
        if (schedulers.find(attribute) == schedulers.end()) return nullptr;

        return schedulers[attribute];
    }

    std::unique_ptr<ActorScheduler> ActorScheduler::CreateNewScheduler(const Attribute& attribute, size_t thread_count) {
        auto scheduler = std::unique_ptr<ActorScheduler>(new ActorScheduler(attribute, thread_count));

        {
            std::lock_guard<std::mutex> guard(schedulers_lock);
            
            if (schedulers.find(attribute) != schedulers.end()) {
                // TODO Error here
            }

            schedulers[attribute] = scheduler.get();
        }

        return scheduler;
    }

}