#include <iostream>

#include <crow/Actor.hpp>
#include <crow/Application.hpp>
#include <crow/Logging.hpp>
#include <crow/Window.hpp>

class IntActor : public crow::Actor<int> {
public:
    void HandleMessage(std::unique_ptr<int>&& msg) override {
        crow::app::Info("IntActor: {}", *msg);

        crow::actor_scheduler->SendMessage<float>(std::make_unique<float>(*msg));
    }
};

class FloatActor : public crow::Actor<float> {
public:
    void HandleMessage(std::unique_ptr<float>&& msg) override {
        crow::app::Warning("FloatActor: {}", *msg);
    }
};

class ExampleApplication : public crow::Application {
protected:
    size_t GetThreadCount() const override {
        return 4; // Only use 4 threads (Including main)
        //return 1; // Only use main thread
        //return crow::Application::GetThreadCount(); // Use all the cores!
    }

    void OnPreActorSchedulerSetup() override {
        // Setup before actor_manager is init
        crow::SetLoggingFile("log.txt");
    }

    void OnRegisterActors() override {
        crow::actor_scheduler->Register<IntActor>();
        crow::actor_scheduler->Register<FloatActor>();
    }

    void OnPostActorSchedulerSetup() override {
        // Setup after actor_manager is init
        crow::actor_scheduler->EmplaceMessageAs<crow::WindowMessageBase>(crow::WindowSetResolution({200, 200}));
        crow::actor_scheduler->EmplaceMessageAs<crow::WindowMessageBase>(crow::WindowCreate{});
        
        crow::actor_scheduler->EmplaceMessage<float>(3.14);
        crow::actor_scheduler->EmplaceMessage<int>(1);

        crow::actor_scheduler->SendMessage(std::make_unique<int>(69));
    }

    void OnUpdate() override {
        crow::actor_scheduler->EmplaceMessageAs<crow::WindowMessageBase>(crow::WindowShouldClose(
            [&](bool close) {
                if (close) {
                    Exit();
                }
            }
        ));

        crow::actor_scheduler->EmplaceMessageAs<crow::WindowMessageBase>(crow::WindowUpdate{});
    }

    void OnPreActorSchedulerCleanup() override {
        // Cleanup before actor_manager is cleanup
        crow::actor_scheduler->EmplaceMessage<int>(42);
    }

    void OnPostActorSchedulerCleanup() override {
        // Cleanup after actor_manager is cleanup
    }
};

//CROW_APPLICATION(ExampleApplication)

int main() {
    ExampleApplication app;
    app._InternalRun();
    return 0;
}