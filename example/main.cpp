#include <iostream>

#include <crow/Actor.hpp>
#include <crow/Window.hpp>

struct AMessage : public crow::MessageBase {
    int num;
};

struct BMessage : public crow::MessageBase {
    int data;
};

class AActor : public crow::ActorBase<AMessage> {
public:
    void HandleMessage(std::unique_ptr<AMessage> msg) override {
        std::cout << "AActor: " << msg->num << std::endl;
        
        BMessage bmsg;
        bmsg.data = msg->num + 10;

        auto b_scheduler = crow::ActorScheduler::GetScheduler(crow::ATTRIBUTE_ACTOR_RENDERING);

        b_scheduler->SendMessage(std::move(bmsg));
    }
};

class BActor : public crow::ActorBase<BMessage> {
public:
    void HandleMessage(std::unique_ptr<BMessage> msg) override {
        std::cout << "BActor: " << msg->data << std::endl;
    }
};


int main() {
    auto a_scheduler = crow::ActorScheduler::CreateNewScheduler(1);

    a_scheduler->Spawn<AActor>();

    auto b_scheduler = crow::ActorScheduler::CreateNewScheduler(crow::ATTRIBUTE_ACTOR_RENDERING, 1);

    b_scheduler->Spawn<BActor>();

    auto window = crow::Window::CreateWindow();
    window->Create();

    AMessage msg;
    msg.num = 1;

    a_scheduler->SendMessage(std::move(msg));

    while (!window->ShouldClose()) {
        window->Update();
    }
    
    a_scheduler->BlockUntilEmpty();
    b_scheduler->BlockUntilEmpty();
}