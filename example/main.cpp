#include <iostream>

#include <crow/Actor.hpp>
#include <crow/Window.hpp>
#include <crow/Graphics.hpp>

#include <thread>

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

        auto b_scheduler = crow::ActorScheduler::GetScheduler(
            crow::ATTRIBUTE_ACTOR_SCHEDULER_RENDERING);

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
    auto a_scheduler = crow::ActorScheduler::CreateNewScheduler();

    a_scheduler->Spawn<AActor>();

    auto a_processor = std::jthread([&]() { a_scheduler->Run(); });

    auto b_scheduler = crow::ActorScheduler::CreateNewScheduler(
        crow::ATTRIBUTE_ACTOR_SCHEDULER_RENDERING);

    b_scheduler->Spawn<BActor>();

    auto b_processor = std::jthread([&]() { b_scheduler->Run(); });

    auto graphics = crow::Graphics::CreateGraphics();

    auto window = crow::Window::CreateWindow(graphics->api);
    window->SetVSync(true);
    window->Create();

    auto vert_buf = graphics->CreateVertexBuffer();
    
    {
        struct Vert {
            float x, y, z;
        };

        vert_buf->Create(crow::Buffer::Usage::StaticDraw, sizeof(Vert) * 3);
        
        Vert vert[] = {
            {1, 0, 0}, {0, 1, 0}, {0, 0, 1}
        };

        auto verts = vert_buf->MapAs<Vert>(crow::Buffer::Access::WriteOnly);
        verts[0] = vert[0];
        verts[1] = vert[1];
        verts[2] = vert[2];
        vert_buf->Unmap();
    }

    auto elem_buffer = graphics->CreateElementBuffer();
    
    {
        elem_buffer->Create(crow::Buffer::Usage::StaticDraw, sizeof(unsigned int) * 3);

        unsigned int inds[] = {
            0, 1, 2
        };

        auto elems = elem_buffer->MapAs<unsigned int>(crow::Buffer::Access::WriteOnly);
        elems[0] = inds[0];
        elems[1] = inds[1];
        elems[2] = inds[2];
        elem_buffer->Unmap();
    }

    auto uniform_buffer = graphics->CreateUniformBuffer();

    {
        uniform_buffer->Create(crow::Buffer::Usage::StaticDraw, sizeof(float) * 4);

        float data[] = {
            1.0f, 2.0f, 3.0f, 4.0f
        };

        auto buffer = uniform_buffer->MapAs<float>(crow::Buffer::Access::WriteOnly);
        buffer[0] = data[0];
        buffer[1] = data[1];
        buffer[2] = data[2];
        buffer[3] = data[3];
        uniform_buffer->Unmap();
    }

    auto ss_buffer = graphics->CreateShaderStorageBuffer();

    {
        ss_buffer->Create(crow::Buffer::Usage::StaticDraw, sizeof(float) * 4);

        float data[] = {
            5, 6, 7, 8
        };

        auto buffer = ss_buffer->MapAs<float>(crow::Buffer::Access::WriteOnly);
        buffer[0] = data[0];
        buffer[1] = data[1];
        buffer[2] = data[2];
        buffer[3] = data[3];
        ss_buffer->Unmap();
    }

    AMessage msg;
    msg.num = 1;

    a_scheduler->SendMessage(std::move(msg));

    while (!window->ShouldClose()) { window->Update(); }

    a_scheduler->BlockUntilEmpty();
    b_scheduler->BlockUntilEmpty();

    a_scheduler->Stop();
    b_scheduler->Stop();
}