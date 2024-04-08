#include <crow/Application.hpp>

#include <crow/Actor.hpp>
#include <crow/Window.hpp>

#include <thread>

namespace crow {

    size_t Application::GetThreadCount() const {
        return std::thread::hardware_concurrency();
    }

    void Application::_InternalRun() {
        OnPreActorSchedulerSetup();

        actor_scheduler = ActorScheduler::Create(GetThreadCount());

        // Register internal actor types
        actor_scheduler->Register<Window>();

        OnRegisterActors();

        OnPostActorSchedulerSetup();

        while (running) {
            OnUpdate();
            actor_scheduler->ProcessAllMessages();
        }

        OnPreActorSchedulerCleanup();

        actor_scheduler->ProcessAllMessages();

        actor_scheduler = nullptr;

        OnPostActorSchedulerCleanup();
    }
}