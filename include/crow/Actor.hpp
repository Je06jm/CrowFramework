#ifndef CROW_ACTOR_HPP
#define CROW_ACTOR_HPP

#include <vector>
#include <mutex>
#include <type_traits>
#include <unordered_map>

#include "Attribute.hpp"

namespace crow {

    struct MessageBase;
    class ActorScheduler;

    using MailboxID = uint32_t;

    inline CROW_ATTRIBUTE(ATTRIBUTE_ACTOR_HIGH_PRIORITY);
    inline CROW_ATTRIBUTE(ATTRIBUTE_ACTOR_MEDIUM_PRIORITY);
    inline CROW_ATTRIBUTE(ATTRIBUTE_ACTOR_LOW_PRIORITY);

    class _InternalActorBase : public AttributeHolder {
        friend class ActorScheduler;
    
    private:
        virtual void Receive(std::unique_ptr<MessageBase> msg) = 0;

        virtual bool IsMailboxEmpty() const = 0;

        virtual void ProcessOneMessage() = 0;

        MailboxID mailbox_id;

        ActorScheduler* scheduler;
    
    public:
        _InternalActorBase(const std::set<Attribute>& attributes) : AttributeHolder(attributes) {}
    };

    template <typename T>
    class ActorBase : public _InternalActorBase {
        friend class ActorScheduler;

        static_assert(std::is_base_of_v<MessageBase, T>);

    private:
        mutable std::mutex mailbox_lock;
        std::vector<std::unique_ptr<T>> mailbox;

        void Receive(std::unique_ptr<T> msg) override {
            std::lock_guard<std::mutex> guard(mailbox_lock);
            mailbox.emplace_back(std::move(msg));
        }

        bool IsMailboxEmpty() const override {
            std::lock_guard<std::mutex> guard(mailbox_lock);
            return mailbox.size() == 0;
        }

        void ProcessOneMessage() override {
            std::unique_lock<std::mutex> guard(mailbox_lock);
            
            if (mailbox.size() != 0) {
                auto msg = std::move(mailbox.front());
                mailbox.erase(mailbox.begin());
                
                guard.unlock();
                HandleMessage(std::move(msg));
            }
        }

    public:
        ActorBase(const std::set<Attribute>& attributes) : _InternalActorBase(attributes) {}
        
        virtual void HandleMessage(std::unique_ptr<T> msg) = 0;
    };

    class ActorScheduler {
    private:
        static std::mutex next_free_mailbox_id_lock;
        static MailboxID next_free_mailbox_id;

        std::mutex actors_lock;
        std::unordered_map<MailboxID, std::unique_ptr<_InternalActorBase>> actors;

        std::mutex mailboxes_lock;
        std::unordered_map<Attribute, std::vector<MailboxID>> mailboxes;

    public:
        template <typename T>
        void SpawnActor();

        template <typename T>
        void SpawnActor(const std::set<Attribute>& attributes);

        void Process();
        void Process(const std::set<Attribute>& attributes);

        void StopProcessing();

        template <typename T>
        void SendMail(MailboxID mailbox, std::unique_ptr<T> msg);
    };

    struct MessageBase : public AttributeHolder {
        MailboxID from;
    };

}

#endif