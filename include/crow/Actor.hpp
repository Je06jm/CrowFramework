#ifndef CROW_ACTOR_HPP
#define CROW_ACTOR_HPP

#include <vector>
#include <mutex>
#include <memory>
#include <atomic>
#include <random>
#include <thread>
#include <type_traits>
#include <typeindex>
#include <unordered_map>

#include "TypeTraits.hpp"
#include "Attribute.hpp"
#include "ReadWriteLock.hpp"
#include "Crow.hpp"

namespace crow {

    CROW_ATTRIBUTE(ATTRIBUTE_ACTOR_RENDERING);
    CROW_ATTRIBUTE(ATTRIBUTE_ACTOR_NON_CRITICAL);
    CROW_ATTRIBUTE(ATTRIBUTE_ACTOR_REGULAR);

    struct MessageBase;
    class ActorScheduler;
    class _InternalActorManagerBase;

    class API _InternalActorBase {
        template <typename T>
        friend class ActorManagerBase;

        template <typename T>
        friend class ActorBase;

        friend class _InternalActorManagerBase;
    
    private:
        _InternalActorManagerBase* manager = nullptr;

        mutable std::mutex process_lock;
        bool is_free_queued = false;

        virtual void Receive(std::unique_ptr<MessageBase> msg) = 0;
        
        virtual bool ProcessOneMessage() = 0;
        virtual bool HasMessages() const = 0;

        virtual void RedistributeMessages() = 0;
    
    protected:
        void QueueFree() {
            is_free_queued = true;
        }

        template <typename T>
        void Spawn();

        template <typename T>
        void SendMessage(T&& msg);

        ActorScheduler* scheduler = nullptr;
    
    public:
        virtual ~_InternalActorBase() {}
    };

    class API _InternalActorManagerBase {
        friend class ActorScheduler;

        template <typename T>
        friend class ActorBase;

    private:
        virtual void Receive(std::unique_ptr<MessageBase> msg) = 0;
        virtual bool HasActors() const = 0;
        virtual bool ProcessOneMessage() = 0;
        virtual bool HasMessages() const = 0;

    protected:
        ActorScheduler* scheduler = nullptr;
    
    public:
        virtual ~_InternalActorManagerBase() {}

        virtual void SpawnOne() = 0;

        template <typename T>
        void Spawn();

        template <typename T>
        void SendMessage(T&& msg);
    };

    template <typename T>
    class API ActorBase : public _InternalActorBase {
        static_assert(std::is_base_of_v<MessageBase, T>);

        static_assert(std::is_move_constructible_v<T>);
        static_assert(std::is_move_assignable_v<T>);

        template <typename J>
        friend class ActorManagerBase;

        template <typename J>
        friend class ActorBase;
    
    private:
        std::vector<std::unique_ptr<T>> mailbox;

        void Receive(std::unique_ptr<MessageBase> msg) override {
            std::lock_guard<std::mutex> guard(process_lock);
            auto derived_msg = std::unique_ptr<T>(static_cast<T*>(msg.release()));
            mailbox.emplace_back(std::move(derived_msg));
        }

        bool ProcessOneMessage() override {
            process_lock.lock();
            if (!is_free_queued) {
                if (mailbox.size() != 0) {
                    auto msg = std::move(mailbox.front());
                    mailbox.erase(mailbox.begin());

                    HandleMessage(std::move(msg));

                    process_lock.unlock();
                    return true;
                }
            }
            process_lock.unlock();

            return false;
        }

        bool HasMessages() const override {
            std::lock_guard<std::mutex> guard(process_lock);
            return mailbox.size() != 0;
        }

        void RedistributeMessages() override {
            process_lock.lock();

            for (size_t i = 0; i < mailbox.size(); i++) {
                auto msg = std::move(mailbox[i]);
                
                if (!manager->HasActors()) break;

                manager->Receive(std::move(msg));
            }
        }

    protected:
        virtual void HandleMessage(std::unique_ptr<T> msg) = 0;
    
    public:
        virtual ~ActorBase() {}
    };

    template <typename T>
    class API ActorManagerBase : public _InternalActorManagerBase {
        template <typename J>
        friend class ActorBase;
    
    private:
        using MessageType = InheritedDataType<T>;

        static_assert(std::is_base_of_v<ActorBase<MessageType>, T>);

        mutable std::mutex actors_lock;
        std::vector<std::unique_ptr<_InternalActorBase>> actors;

        std::random_device rd;
        std::mt19937 gen;

        void Receive(std::unique_ptr<MessageBase> msg) override {
            std::lock_guard<std::mutex> guard(actors_lock);

            std::uniform_int_distribution<size_t> rand_actor(0, actors.size() - 1);
            
            size_t index = rand_actor(gen);
            actors[index]->Receive(std::move(msg));
        }

        bool HasActors() const override {
            std::lock_guard<std::mutex> guard(actors_lock);
            return actors.size() != 0;
        }

        bool ProcessOneMessage() override {
            std::unique_lock<std::mutex> guard(actors_lock);

            for (size_t i = 0; i < actors.size(); i++) {
                auto actor = actors[i].get();

                guard.unlock();

                if (actor->ProcessOneMessage()) {
                    guard.lock();
                    
                    if (actor->is_free_queued) {
                        for (size_t j = 0; j < actors.size(); j++) {
                            if (actors[j].get() == actor) {
                                auto moved_actor = std::move(actors[j]);
                                actors.erase(actors.begin() + j);

                                guard.unlock();

                                actor->RedistributeMessages();
                            }
                        }
                    }

                    return true;
                } else {
                    guard.lock();
                }
            }

            return false;
        }

        bool HasMessages() const override {
            {
                std::lock_guard<std::mutex> guard(actors_lock);

                for (size_t i = 0; i < actors.size(); i++) {
                    if (actors[i]->HasMessages()) {
                        return true;
                    }
                }
            }

            return false;
        }
    
    public:
        void SpawnOne() override {
            auto actor = std::unique_ptr<_InternalActorBase>(new T);
            
            actor->manager = static_cast<_InternalActorManagerBase*>(this);
            actor->scheduler = scheduler;

            std::lock_guard<std::mutex> guard(actors_lock);
            actors.emplace_back(std::move(actor));
        }
    };

    class API ActorScheduler {
    private:
        static std::mutex schedulers_lock;
        static std::unordered_map<Attribute, ActorScheduler*> schedulers;

        mutable ReadWriteLock managers_lock;
        std::unordered_map<std::type_index, std::unique_ptr<_InternalActorManagerBase>> managers;

        std::vector<std::thread> threads;

        std::random_device rd;
        std::mt19937 gen;

        bool running = true;

        ActorScheduler(const Attribute& attribute, size_t thread_count);

        bool ProcessOneMessage();
    public:
        const Attribute& attribute;
        const size_t thread_count;

        ~ActorScheduler();

        template <typename T>
        void Spawn() {
            using MessageType = InheritedDataType<T>;

            static_assert(std::is_base_of_v<ActorBase<MessageType>, T>);

            std::type_index index{typeid(MessageType)};

            managers_lock.LockReading();
            if (managers.find(index) == managers.end()) {
                managers_lock.UnlockReading();
                managers_lock.LockWriting();

                managers[index] = std::unique_ptr<_InternalActorManagerBase>(new ActorManagerBase<T>);
                managers[index]->scheduler = this;

                managers_lock.UnlockWriting();
                managers_lock.LockReading();
            }

            managers[index]->SpawnOne();
            managers_lock.UnlockReading();
        }

        inline void Stop() {
            running = false;
        }

        template <typename T>
        void SendMessage(T&& message) {
            static_assert(std::is_base_of_v<MessageBase, T>);
            
            std::type_index index{typeid(T)};

            auto msg = std::unique_ptr<MessageBase>(new T(std::move(message)));

            managers_lock.LockReading();

            if (managers.find(index) == managers.end()) {
                // TODO What to do here?
                return;
            }

            if (!managers[index]->HasActors()) {
                // TODO What to do here?
                return;
            }

            managers[index]->Receive(std::move(msg));
            managers_lock.UnlockReading();
        }

        void Run(bool until_empty = false);
        
        void BlockUntilEmpty() const;

        static ActorScheduler* GetScheduler(const Attribute& attribute);

        static std::unique_ptr<ActorScheduler> CreateNewScheduler(const Attribute& attribute, size_t thread_count);
        
        inline static std::unique_ptr<ActorScheduler> CreateNewScheduler(size_t thread_count) {
            return CreateNewScheduler(ATTRIBUTE_ACTOR_REGULAR, thread_count);
        }
    };

    template <typename T>
    void _InternalActorBase::Spawn() {
        manager->Spawn<T>();
    }

    template <typename T>
    void _InternalActorBase::SendMessage(T&& msg) {
        manager->SendMessage<T>(std::move(msg));
    }

    template <typename T>
    void _InternalActorManagerBase::Spawn() {
        scheduler->Spawn<T>();
    }

    template <typename T>
    void _InternalActorManagerBase::SendMessage(T&& msg) {
        scheduler->SendMessage(std::move(msg));
    }

    struct MessageBase {};

}

#endif