#ifndef MQMX_MESSAGE_QUEUE_H_INCLUDED
#define MQMX_MESSAGE_QUEUE_H_INCLUDED 1

#include <memory>
#include <mutex>
#include <deque>

#include <mqmx/message.h>

namespace mqmx
{
    class message_queue
    {
        message_queue (const message_queue &) = delete;
        message_queue & operator = (const message_queue &) = delete;

    public:
        typedef std::unique_ptr<message>     message_ptr_type;
        typedef std::mutex                   mutex_type;
        typedef std::unique_lock<mutex_type> lock_type;
        typedef std::deque<message_ptr_type> container_type;
        typedef size_t                       notification_flags_type;

        enum NotificationFlag
        {
            NewData  = 0x0001,        /* push operation called on this queue */
            Detached = 0x0002,        /* move ctor or move assignment called on this queue */
            Closed   = 0x0004         /* destructor called on this queue */
        };

        struct listener
        {
            virtual ~listener () { }
            virtual void notify (const queue_id_type,
                                 message_queue *,
                                 const notification_flags_type) noexcept = 0;
        };

    public:
        message_queue (const queue_id_type = message::undefined_qid);
        message_queue (message_queue &&);
        ~message_queue ();

        queue_id_type get_id () const;

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
