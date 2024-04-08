#ifndef CROW_APPLICATION_HPP
#define CROW_APPLICATION_HPP

#include <memory>

#include "Crow.hpp"

namespace crow {

    class API Application {
    private:
        bool running = true;

    public:
        Application() = default;
        virtual ~Application() = default;

    protected:
        inline void Exit() { running = false; }

        virtual size_t GetThreadCount() const;

        virtual void OnPreActorSchedulerSetup() = 0;
        virtual void OnPostActorSchedulerSetup() = 0;
        virtual void OnRegisterActors() = 0;
        virtual void OnUpdate() = 0;
        virtual void OnPreActorSchedulerCleanup() = 0;
        virtual void OnPostActorSchedulerCleanup() = 0;
    
    public:
        void _InternalRun();
    };
}

// TODO: Process command ling arguments??
#define CROW_APPLICATION(App)\
int main(int argc, const char** argv) {\
    App app;\
    app._InternalRun();\
    return 0;\
}

#endif