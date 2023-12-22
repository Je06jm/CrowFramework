#ifndef CROW_ACTOR_HPP
#define CROW_ACTOR_HPP

#include <atomic>
#include <memory>
#include <mutex>
#include <random>
#include <thread>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "Attribute.hpp"
#include "Crow.hpp"
#include "ReadWriteLock.hpp"
#include "TypeTraits.hpp"

namespace crow {

    /// @brief Indicates that an actor scheduler handles rendering messages
    CROW_ATTRIBUTE(ATTRIBUTE_ACTOR_SCHEDULER_RENDERING);

    /// @brief Indicates that an actor scheduler handles regular messages
    CROW_ATTRIBUTE(ATTRIBUTE_ACTOR_SCHEDULER_REGULAR);

    struct MessageBase;
    class ActorScheduler;
    class _InternalActorManagerBase;

    /// @brief This is an internal class. DO NOT USE! This class is the
    /// foundation of all actors
    class API _InternalActorBase {
        template <typename T>
        friend class ActorManagerBase;

        template <typename T>
        friend class ActorBase;

        friend class _InternalActorManagerBase;

    private:
        /// @brief The manager that manages this actor
        _InternalActorManagerBase* manager = nullptr;

        /// @brief This is locked whenever the actor is being modified or if
        /// a message is being processed
        mutable std::mutex process_lock;

        /// @brief When set to true, no more messages can be received. Also, the
        /// scheduler will call RedistributeMessages then free the actor
        bool is_free_queued = false;

        /// @brief This will place new messages into the actor's mailbox
        /// @param msg The message to place in the mailbox
        virtual void Receive(std::unique_ptr<MessageBase> msg) = 0;

        /// @brief This will process the first message in the mailbox
        /// @return \c true if a message was processed, \c false otherwise
        virtual bool ProcessOneMessage() = 0;

        /// @brief This determins if the actor has messages
        /// @return \c true if there is at least one message, \c false otherwise
        virtual bool HasMessages() const = 0;

        /// @brief This redistributes all the messages in the actor's mailbox to
        /// all of the actors of the same type. This is only called right before
        /// the actor if freed
        virtual void RedistributeMessages() = 0;

    protected:
        /// @brief Queue the actor to be freed
        void QueueFree() { is_free_queued = true; }

        /// @brief Spawn a new actor using the same scheduler that owns the
        /// actor
        /// @tparam T The actor type to spawn
        template <typename T>
        void Spawn();

        /// @brief Sends a message to any actor in the same scheduler that
        /// accepts the message type
        /// @tparam T The message type
        /// @param msg The message that is moved
        template <typename T>
        void SendMessage(T&& msg);

        /// @brief The scheduler that owns the actor
        ActorScheduler* scheduler = nullptr;

    public:
        /// @brief Does nothing, just a virtual deconstructor
        virtual ~_InternalActorBase() {}
    };

    /// @brief This is an internal class. DO NOT USE! This is the
    /// foundation of all actor managers
    class API _InternalActorManagerBase {
        friend class ActorScheduler;

        template <typename T>
        friend class ActorBase;

    private:
        /// @brief This will place new messages into an actor's mailbox. The
        /// actor is selected at random
        /// @param msg The message to place in the actor's mailbox
        virtual void Receive(std::unique_ptr<MessageBase> msg) = 0;

        /// @brief This determins if there are any actors in the manager
        /// @return \c true if there is at least one actor, \c false otherwise
        virtual bool HasActors() const = 0;

        /// @brief This will process the first message in an actor's mailbox.
        /// The actor is selected beginning with the first until an actor with
        /// any messages is found
        /// @return \c true if a message was processed, \c false otherwise
        virtual bool ProcessOneMessage() = 0;

        /// @brief This determins if at least one actor has at least one message
        /// @return \c true if there is at least one message, \c false otherwise
        virtual bool HasMessages() const = 0;

    protected:
        /// @brief The owning scheduler
        ActorScheduler* scheduler = nullptr;

    public:
        /// @brief Does nothing, just a virtual deconstructor
        virtual ~_InternalActorManagerBase() {}

        /// @brief Spawns one actor that can handle the same message type as the
        /// manager is assigned to
        virtual void SpawnOne() = 0;

        /// @brief Spawn a new actor using the same scheduler that owns the
        /// manager
        /// @tparam T The actor type to spawn
        template <typename T>
        void Spawn();

        /// @brief Sends a message to any actor in the same scheduler that
        /// accepts the message type
        /// @tparam T The message type
        /// @param msg The message that is moved
        template <typename T>
        void SendMessage(T&& msg);
    };

    /// @brief The actor base class. The actor can accept \c T typed messages
    /// and can process them on any thread
    /// @tparam T The message type
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
        /// @brief A list of messages
        std::vector<std::unique_ptr<T>> mailbox;

        /// @brief This will place new messages into the actor's mailbox
        /// @param msg The message to place in the mailbox
        void Receive(std::unique_ptr<MessageBase> msg) override {
            std::lock_guard<std::mutex> guard(process_lock);
            auto derived_msg =
                std::unique_ptr<T>(static_cast<T*>(msg.release()));
            mailbox.emplace_back(std::move(derived_msg));
        }

        /// @brief This will process the first message in the mailbox
        /// @return \c true if a message was processed, \c false otherwise
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

        /// @brief This determins if the actor has messages
        /// @return \c true if there is at least one message, \c false otherwise
        bool HasMessages() const override {
            std::lock_guard<std::mutex> guard(process_lock);
            return mailbox.size() != 0;
        }

        /// @brief This redistributes all the messages in the actor's mailbox to
        /// all of the actors of the same type. This is only called right before
        /// the actor if freed
        void RedistributeMessages() override {
            process_lock.lock();

            for (size_t i = 0; i < mailbox.size(); i++) {
                auto msg = std::move(mailbox[i]);

                if (!manager->HasActors()) break;

                manager->Receive(std::move(msg));
            }
        }

    protected:
        /// @brief A virtual method which is called whenever a message needs to
        /// be processed
        /// @param msg The message to process
        virtual void HandleMessage(std::unique_ptr<T> msg) = 0;

    public:
        /// @brief Does nothing, just a virtual deconstructor
        virtual ~ActorBase() {}
    };

    /// @brief The actor manager base class. The manager can spawn or free \c T
    /// typed actors
    /// @tparam T The actor type
    template <typename T>
    class API ActorManagerBase : public _InternalActorManagerBase {
        template <typename J>
        friend class ActorBase;

    private:
        /// @brief The type of message that the actors can accept
        using MessageType = InheritedDataType<T>;

        static_assert(std::is_base_of_v<ActorBase<MessageType>, T>);

        /// @brief This is locked whenever an actor needs to be spawned / freed
        mutable std::mutex actors_lock;

        /// @brief The list of spawned actors
        std::vector<std::unique_ptr<_InternalActorBase>> actors;

        /// @brief Used to determine a random actor
        std::random_device rd;

        /// @brief Used to determine a random actor
        std::mt19937 gen;

        /// @brief This will place new messages into an actor's mailbox. The
        /// actor is selected at random
        /// @param msg The message to place in the actor's mailbox
        void Receive(std::unique_ptr<MessageBase> msg) override {
            std::lock_guard<std::mutex> guard(actors_lock);

            std::uniform_int_distribution<size_t> rand_actor(0,
                                                             actors.size() - 1);

            size_t index = rand_actor(gen);
            actors[index]->Receive(std::move(msg));
        }

        /// @brief This determins if there are any actors in the manager
        /// @return \c true if there is at least one actor, \c false otherwise
        bool HasActors() const override {
            std::lock_guard<std::mutex> guard(actors_lock);
            return actors.size() != 0;
        }

        /// @brief This will process the first message in an actor's mailbox.
        /// The actor is selected beginning with the first until an actor with
        /// any messages is found
        /// @return \c true if a message was processed, \c false otherwise
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

        /// @brief This determins if at least one actor has at least one message
        /// @return \c true if there is at least one message, \c false otherwise
        bool HasMessages() const override {
            {
                std::lock_guard<std::mutex> guard(actors_lock);

                for (size_t i = 0; i < actors.size(); i++) {
                    if (actors[i]->HasMessages()) { return true; }
                }
            }

            return false;
        }

    public:
        /// @brief Spawns one actor that can handle the same message type as the
        /// manager is assigned to
        void SpawnOne() override {
            auto actor = std::unique_ptr<_InternalActorBase>(new T);

            actor->manager = static_cast<_InternalActorManagerBase*>(this);
            actor->scheduler = scheduler;

            std::lock_guard<std::mutex> guard(actors_lock);
            actors.emplace_back(std::move(actor));
        }
    };

    /// @brief The actor scheduler class. This schedule which actor is processed
    /// in which threads
    class API ActorScheduler {
    private:
        /// @brief This is locked whenever a scheduler is created
        static std::mutex schedulers_lock;

        /// @brief This holds a list of which schedulers are associated with
        /// which Attributes
        static std::unordered_map<Attribute, ActorScheduler*> schedulers;

        /// @brief This is locked whenever a manager is created / destroyed
        mutable ReadWriteLock managers_lock;

        /// @brief This holds a list of which managers are associated with
        /// which message type
        std::unordered_map<std::type_index,
                           std::unique_ptr<_InternalActorManagerBase>>
            managers;

        /// @brief A list of threads
        std::vector<std::thread> threads;

        /// @brief Used to determine a random manager
        std::random_device rd;

        /// @brief Used to determine a random manager
        std::mt19937 gen;

        /// @brief When set to \c false, the threads exit
        bool running = true;

        /// @brief Set the const variables. Also spawn the threads
        /// @param attribute The attribute that is associated with the scheduler
        /// @param thread_count The number of threads to spawn
        ActorScheduler(const Attribute& attribute, size_t thread_count);

        /// @brief This will process a message from the managers beginning with
        /// the first manager
        /// @return \c true is a message was processed, \c false otherwise
        bool ProcessOneMessage();

    public:
        /// @brief The attribute that is associated with the scheduler
        const Attribute& attribute;

        /// @brief The number of threads that were spawned by the scheduler
        const size_t thread_count;

        /// @brief Stops and joins the threads
        ~ActorScheduler();

        /// @brief Spawns a new actor and put it in it's owning manager
        /// @tparam T The actor type to spawn
        template <typename T>
        void Spawn() {
            using MessageType = InheritedDataType<T>;

            static_assert(std::is_base_of_v<ActorBase<MessageType>, T>);

            std::type_index index{typeid(MessageType)};

            managers_lock.LockReading();
            if (managers.find(index) == managers.end()) {
                managers_lock.UnlockReading();
                managers_lock.LockWriting();

                managers[index] = std::unique_ptr<_InternalActorManagerBase>(
                    new ActorManagerBase<T>);
                managers[index]->scheduler = this;

                managers_lock.UnlockWriting();
                managers_lock.LockReading();
            }

            managers[index]->SpawnOne();
            managers_lock.UnlockReading();
        }

        /// @brief Signal the scheduler to stop and join the threads
        inline void Stop() {
            running = false;
            for (auto& thread : threads) { thread.join(); }
        }

        /// @brief Sends a message to any actor owned by the scheduler that
        /// accepts the message type
        /// @tparam T The message type
        /// @param msg The message that is moved
        template <typename T>
        void SendMessage(T&& msg) {
            static_assert(std::is_base_of_v<MessageBase, T>);

            std::type_index index{typeid(T)};

            auto moved_msg =
                std::unique_ptr<MessageBase>(new T(std::move(msg)));

            managers_lock.LockReading();

            if (managers.find(index) == managers.end()) {
                // TODO What to do here?
                return;
            }

            if (!managers[index]->HasActors()) {
                // TODO What to do here?
                return;
            }

            managers[index]->Receive(std::move(moved_msg));
            managers_lock.UnlockReading();
        }

        /// @brief This allows any thread to process messages
        /// @param until_empty \c true will return when no messages are
        /// processed, \c false otherwise
        void Run(bool until_empty = false);

        /// @brief Blocks the calling thread until there are no more messages
        void BlockUntilEmpty() const;

        /// @brief Returns the scheduler associated with an Attribute
        /// @param attribute The Attribute
        /// @return The scheduler
        static ActorScheduler* GetScheduler(const Attribute& attribute);

        /// @brief Creates a new scheduler that is associated with the
        /// attribute. \c thread_count threads will also be spawned
        /// @param attribute The Attribute
        /// @param thread_count The number of threads to spawn
        /// @return A pointer to the scheduler
        static std::unique_ptr<ActorScheduler>
        CreateNewScheduler(const Attribute& attribute, size_t thread_count);

        /// @brief This is the same as
        /// \code CreateNewScheduler(const Attribute&, size_t); \endcode except
        /// the Attribute is set to \c ATTRIBUTE_ACTOR_SCHEDULER_REGULAR
        /// @param thread_count The number of threads to spawn
        /// @return A pointer to the scheduler
        inline static std::unique_ptr<ActorScheduler>
        CreateNewScheduler(size_t thread_count) {
            return CreateNewScheduler(ATTRIBUTE_ACTOR_SCHEDULER_REGULAR,
                                      thread_count);
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

    struct API MessageBase {};

}

#endif