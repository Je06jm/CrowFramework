#include <crow/Actor.hpp>

#ifdef WINDOWS
#include <Windows.h>
#else
#include <sched.h>
#endif

namespace crow {

    CROW_DEFINE_ATTRIBUTE(ATTRIBUTE_ACTOR_SCHEDULER_RENDERING);
    CROW_DEFINE_ATTRIBUTE(ATTRIBUTE_ACTOR_SCHEDULER_NON_CRITICAL);
    CROW_DEFINE_ATTRIBUTE(ATTRIBUTE_ACTOR_SCHEDULER_REGULAR);

    std::mutex ActorScheduler::schedulers_lock;
    std::unordered_map<Attribute, ActorScheduler*> ActorScheduler::schedulers;

    ActorScheduler::ActorScheduler(const Attribute& attribute)
        : attribute{attribute} {
        gen = std::mt19937(rd());
    }

    ActorScheduler::~ActorScheduler() {}

    bool ActorScheduler::ProcessOneMessage() {
        if (!running) return false;

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

    std::unique_ptr<ActorScheduler>
    ActorScheduler::CreateNewScheduler(const Attribute& attribute) {
        auto scheduler =
            std::unique_ptr<ActorScheduler>(new ActorScheduler(attribute));

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