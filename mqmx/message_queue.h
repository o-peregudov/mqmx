#pragma once

#include <mutex>
#include <deque>
#include <type_traits>

#include <mqmx/Message.h>

namespace mqmx
{
    class message_queue
    {
        message_queue (const message_queue &) = delete;
        message_queue & operator = (const message_queue &) = delete;

    public:
        typedef std::unique_ptr<message_queue>     upointer_type;
        typedef std::mutex                         mutex_type;
        typedef std::unique_lock<mutex_type>       lock_type;
        typedef std::deque<Message::upointer_type> container_type;
        typedef size_t                             notification_flags_type;

        enum notification_flag
        {
            NewData  = 0x0001, /* push operation called on this queue */
            Detached = 0x0002, /* move ctor or move assignment called on this queue */
            Closed   = 0x0004  /* destructor called on this queue */
        };

        struct listener
        {
            virtual ~listener () { }
            virtual void notify (const queue_id_type,
                                 message_queue *,
                                 const notification_flags_type) = 0;
        };

    public:
        message_queue (const queue_id_type = Message::UndefinedQID);
        ~message_queue ();

        message_queue (message_queue &&);
        message_queue & operator = (message_queue &&);

        queue_id_type getQID () const;

        status_code push (Message::upointer_type &&);
        Message::upointer_type pop ();

        template <typename MessageType, typename... ParametersTypes>
        Message::upointer_type new_message (ParametersTypes&&... args) const
        {
            static_assert (std::is_base_of<Message, MessageType>::value,
			   "Invalid MessageType - should be derived from mqmx::Message");
            return Message::upointer_type (
                new MessageType (getQID (), std::forward<ParametersTypes> (args)...));
        }

        template <typename MessageType, typename... ParametersTypes>
        status_code enqueue (ParametersTypes&&... args)
        {
            static_assert (std::is_base_of<Message, MessageType>::value,
			   "Invalid MessageType - should be derived from mqmx::Message");
            return push (new_message<MessageType> (std::forward<ParametersTypes> (args)...));
        }

    public:
        status_code set_listener (listener &);
        void clear_listener ();

    private:
        queue_id_type  m_id;
        mutex_type     m_mutex;
        container_type m_queue;
        listener *     m_listener;
    };
} /* namespace mqmx */
