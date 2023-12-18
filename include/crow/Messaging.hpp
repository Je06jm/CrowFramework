#ifndef CROW_MESSAGING_HPP
#define CROW_MESSAGING_HPP

#include <vector>
#include <mutex>
#include <functional>
#include <type_traits>
#include <unordered_map>

#include "Thread.hpp"

namespace crow {

    struct Message;
    
    class MessageQueue {
        friend Message;
    
    public:
        using ID = uint32_t;

        static constexpr ID INVALID = -1U;
    
    private:
        mutable std::mutex messages_lock;
        std::vector<std::unique_ptr<Message>> messages;

        static ID next_free_id;
        static std::mutex queues_lock;
        static std::unordered_map<ID, MessageQueue*> queues;

    public:
        const ID id;

        MessageQueue() : id{next_free_id++} {
            std::lock_guard<std::mutex> guard(queues_lock);
            queues[id] = this;
        }

        ~MessageQueue() {
            std::lock_guard<std::mutex> guard(queues_lock);
            queues.erase(id);
        }

        template <typename T>
        void PushMessage(T&& message) {
            static_assert(std::is_trivially_copy_constructible_v<T>);

            std::lock_guard<std::mutex> guard(messages_lock);
            messages.emplace_back(std::unique_ptr(new T(message)));
        }

        std::unique_ptr<Message> PopMessage() {
            std::lock_guard<std::mutex> guard(messages_lock);
            
            if (messages.size() != 0) {
                auto message = std::move(messages.front());
                messages.erase(messages.begin());
                return message;
            } else {
                auto message = CreateEmptyMessage<Message>();
                return message;
            }
        }

        bool HasMessages() const {
            std::lock_guard<std::mutex> guard(messages_lock);
            return messages.size() != 0;
        }

        void RemoveAllMessages() {
            std::lock_guard<std::mutex> guard(messages_lock);
            messages.clear();
        }

        template <typename T>
        std::unique_ptr<T> CreateEmptyMessage();

        static MessageQueue* GetQueue(ID id) {
            std::lock_guard<std::mutex> guard(queues_lock);
            
            if (queues.find(id) != queues.end()) {
                return queues[id];
            } else {
                return nullptr;
            }
        }
    };

    struct Message {
        template <typename T>
        friend class MessageQueue;

        struct From {
            MessageQueue::ID queue;
            Thread::ID thread;
        } from;

        struct To {
            MessageQueue::ID queue;
            Thread::ID thread;
        } to;

        static constexpr From FROM_UNKNOWN = {MessageQueue::INVALID, Thread::INVALID};
        static constexpr To TO_ANY = {MessageQueue::INVALID, Thread::INVALID};

        Message(const Message&) = delete;
        Message& operator=(const Message&) = delete;

        Message(Message&& message) {
            from = message.from;
            to = message.to;

            message.from = FROM_UNKNOWN;
            message.to = TO_ANY;
        }

        Message& operator=(Message&& message) {
            from = message.from;
            to = message.to;

            message.from = FROM_UNKNOWN;
            message.to = TO_ANY;

            return *this;
        }
    
    private:
        Message() {}

    };

    template <typename T>
    std::unique_ptr<T> MessageQueue::CreateEmptyMessage() {
        static_assert(std::is_base_of_v<Message, T>);
        
        auto message = std::unique_ptr<Message>(new T);
        
        message->from.queue = id;
        message->from.thread = Thread::GetCurrentThreadID();
        message->to = Message::TO_ANY;

        return message;
    }

}

#endif