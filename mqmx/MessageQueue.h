#pragma once

#include <memory>
#include <mutex>
#include <deque>

#include <mqmx/Message.h>

namespace mqmx
{
    class MessageQueue
    {
        MessageQueue (const MessageQueue &) = delete;
        MessageQueue & operator = (const MessageQueue &) = delete;

    public:
        typedef std::unique_ptr<Message>     message_ptr_type;
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

        struct Listener
        {
            virtual ~Listener () { }
            virtual void notify (const queue_id_type,
                                 MessageQueue *,
                                 const notification_flags_type) noexcept = 0;
        };

    public:
        MessageQueue (const queue_id_type = Message::UndefinedQID);
        ~MessageQueue ();

        MessageQueue (MessageQueue &&);
	MessageQueue & operator = (MessageQueue &&);

        queue_id_type getQID () const;

        status_code push (message_ptr_type && msg);
        message_ptr_type pop ();

    public:
        status_code setListener (Listener &);
        void clearListener ();

    private:
        queue_id_type  _id;
        mutex_type     _mutex;
        container_type _queue;
        Listener *     _listener;
    };
} /* namespace mqmx */
