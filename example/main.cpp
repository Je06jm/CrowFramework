#include <iostream>

#include <crow/Actor.hpp>
#include <crow/ReadWriteLock.hpp>

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

//crow::ActorScheduler scheduler;

int main() {
    std::cout << "I'm running" << std::endl;

    auto a_scheduler = crow::ActorScheduler::CreateNewScheduler(1);

    a_scheduler->Spawn<AActor>();

    auto b_scheduler = crow::ActorScheduler::CreateNewScheduler(crow::ATTRIBUTE_ACTOR_RENDERING, 1);

    b_scheduler->Spawn<BActor>();

    AMessage msg;
    msg.num = 1;

    a_scheduler->SendMessage(std::move(msg));

    //scheduler->Run(true);
    a_scheduler->BlockUntilEmpty();
    b_scheduler->BlockUntilEmpty();
}