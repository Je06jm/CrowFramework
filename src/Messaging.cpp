#include <crow/Messaging.hpp>

namespace crow {

    MessageQueue::ID MessageQueue::next_free_id = 0;
    std::mutex MessageQueue::queues_lock;
    std::unordered_map<MessageQueue::ID, MessageQueue*> MessageQueue::queues;

}