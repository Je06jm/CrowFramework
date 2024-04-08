#ifndef CROW_ACTOR_HPP
#define CROW_ACTOR_HPP

#include <atomic>
#include <memory>
#include <mutex>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <list>
#include <thread>

#include "Crow.hpp"
#include "Logging.hpp"

namespace crow {

    class API _InternalActorBase {
        friend class ActorScheduler;
    protected:
        virtual void ProcessMessage() = 0;

    public:
        virtual ~_InternalActorBase() = default;
        
        virtual bool MainThreadOnly() const { return false; }
    };

    template <typename T>
    class API Actor : public _InternalActorBase {
        static_assert(std::is_move_assignable_v<T> || std::is_move_constructible_v<T>);

        friend class ActorScheduler;

    private:
        std::mutex lock;
        std::vector<std::unique_ptr<T>> mailbox;
    
    public:
        using MessageType = T;

        virtual ~Actor() = default;

        virtual void HandleMessage(std::unique_ptr<T>&& msg) = 0;
    
    protected:
        void AcceptMessage(std::unique_ptr<T>&& msg) {
            lock.lock();
            mailbox.push_back(std::move(msg));
            lock.unlock();
        }

        void ProcessMessage() override {
            lock.lock();

            auto msg = std::move(mailbox.front());
            mailbox.erase(mailbox.begin());

            lock.unlock();

            HandleMessage(std::move(msg));
        };
    };

    class API ActorScheduler {
    private:
        using ActorPtr = std::shared_ptr<_InternalActorBase>;

        bool running = true;

        std::mutex lock;

        std::unordered_map<std::type_index, ActorPtr> actors;
        std::unordered_map<std::type_index, bool> actors_is_main;

        std::list<ActorPtr> to_do;
        std::list<ActorPtr> main_to_do;

        std::vector<std::thread> threads;

        ActorScheduler(size_t thread_count);

        void YieldCPU() const;

        bool ProcessMessage(bool is_main = false);

        std::atomic_size_t working = 0;

    public:
        ~ActorScheduler();

        template <typename T>
        void Register() {
            static_assert(std::is_base_of_v<_InternalActorBase, T>);

            using Type = T::MessageType;

            auto index = std::type_index(typeid(Type));

            lock.lock();

            if (actors.find(index) != actors.end()) {
                lock.unlock();

                engine::Critical("Cannot register Actor {} more than once", typeid(T).name());
            }

            auto actor = ActorPtr(new T);
            bool is_main = actor->MainThreadOnly();

            actors[index] = actor;
            actors_is_main[index] = is_main;
            lock.unlock();
        }

        template <typename T>
        bool SendMessage(std::unique_ptr<T>&& msg) {
            auto index = std::type_index(typeid(T));

            lock.lock();
            if (actors.find(index) == actors.end()) {
                lock.unlock();
                return false;
            }

            auto actor = actors[index];
            auto is_main = actors_is_main[index];

            lock.unlock();

            auto typed_actor = dynamic_cast<Actor<T>*>(actor.get());

            if (!typed_actor) return false;

            typed_actor->AcceptMessage(std::move(msg));

            lock.lock();

            if (is_main) main_to_do.push_back(actor);
            else to_do.push_back(actor);

            lock.unlock();
            return true;
        }

        template <typename As, typename Type>
        inline bool SendMessageAs(std::unique_ptr<Type>&& msg) {
            std::unique_ptr<As> as(static_cast<As*>(msg.release()));
            return SendMessage(std::move(as));
        }

        template <typename T>
        inline bool EmplaceMessage(T&& msg) {
            static_assert(std::is_move_constructible_v<T>);
            return SendMessage(std::unique_ptr<T>(new T(std::move(msg))));
        }

        template <typename As, typename Type>
        inline bool EmplaceMessageAs(Type&& msg) {
            static_assert(std::is_move_constructible_v<Type>);
            return SendMessageAs<As, Type>(std::unique_ptr<Type>(new Type(std::move(msg))));
        }

        void ProcessAllMessages();

        inline static auto Create(size_t thread_count) {
            return std::unique_ptr<ActorScheduler>(new ActorScheduler(thread_count));
        }
    };

    extern std::unique_ptr<ActorScheduler> API actor_scheduler;

}

#endif