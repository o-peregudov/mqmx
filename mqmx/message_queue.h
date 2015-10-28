#ifndef MQMX_MESSAGE_QUEUE_H_INCLUDED
#define MQMX_MESSAGE_QUEUE_H_INCLUDED 1

#include <memory>
#include <mutex>
#include <deque>

#include <mqmx/message.h>

namespace mqmx
{
    enum MQNotification
    {
        NewMessage,
        Detached
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

        class listener
        {
        public:
            virtual ~listener () { }
            virtual void notify (const queue_id_type,
                                 const message_queue &,
                                 const MQNotification) noexcept = 0;
        };

    public:
        message_queue (const queue_id_type = message::undefined_qid) noexcept;
        message_queue (message_queue && o) noexcept;

        message_queue & operator = (message_queue && o) noexcept;

        status_code push (message_ptr_type && msg);
        message_ptr_type pop ();

    public:
        status_code set_listener (listener &);
        void clear_listener ();

    private:
        queue_id_type  _id;
        mutex_type     _mutex;
        container_type _queue;
        listener *     _listener;
    };
} /* namespace mqmx */
#endif /* MQMX_MESSAGE_QUEUE_H_INCLUDED */
