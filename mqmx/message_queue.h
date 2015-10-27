#ifndef MQMX_MESSAGE_QUEUE_H_INCLUDED
#define MQMX_MESSAGE_QUEUE_H_INCLUDED 1

#include <memory>
#include <mutex>
#include <deque>
#include <functional>

#include <mqmx/message.h>

namespace mqmx
{
    enum MQNotification
    {
        NewMessage
    };

    class message_queue
    {
        message_queue (const message_queue &) = delete;
        message_queue & operator = (const message_queue &) = delete;

    public:
        typedef std::unique_ptr<message>     message_ptr_type;
        typedef std::mutex                   mutex_type;
        typedef std::unique_lock<mutex_type> lock_type;
        typedef std::deque<message_ptr_type> container_type;
        typedef std::function<void (const queue_id_type &, const MQNotification)>
                                             listener_function_type;

    public:
        message_queue (const queue_id_type = message::undefined_qid) noexcept;
        message_queue (message_queue && o) noexcept;

        message_queue & operator = (message_queue && o) noexcept;

        status_code push (message_ptr_type && msg);
        message_ptr_type pop ();

    private:
        queue_id_type          _id;
        mutex_type             _mutex;
        container_type         _queue;
        listener_function_type _listener;
    };
} /* namespace mqmx */
#endif /* MQMX_MESSAGE_QUEUE_H_INCLUDED */
