#pragma once

#include <mqmx/message.h>

#include <crs/mutex.h>

#include <deque>
#include <type_traits>

namespace crs = CrossClass;
namespace mqmx
{
    class message_queue
    {
        message_queue (const message_queue &) = delete;
        message_queue & operator = (const message_queue &) = delete;

    public:
        typedef std::unique_ptr<message_queue>     upointer_type;
        typedef crs::mutex_type                    mutex_type;
        typedef crs::lock_type                     lock_type;
        typedef std::deque<message::upointer_type> container_type;
        typedef size_t                             notification_flags_type;

        enum notification_flag
        {
            data     = 0x0001, /* push operation called on this queue */
            detached = 0x0002, /* move ctor or move assignment called on this queue */
            closed   = 0x0004  /* destructor called on this queue */
        };

        struct listener
        {
            virtual ~listener () { }
            virtual void notify (const queue_id_type,
                                 message_queue *,
                                 const notification_flags_type) = 0;
        };

    public:
        message_queue (const queue_id_type = message::undefined_qid);
        ~message_queue ();

        message_queue (message_queue &&);
        message_queue & operator = (message_queue &&);

        queue_id_type get_qid () const;

        status_code push (message::upointer_type &&);
        message::upointer_type pop ();

        template <typename message_type, typename... parameters>
        message::upointer_type new_message (parameters&&... args) const
        {
            static_assert (std::is_base_of<message, message_type>::value,
			   "Invalid message_type - should be derived from mqmx::message");
            return message::upointer_type (
                new message_type (get_qid (), std::forward<parameters> (args)...));
        }

        template <typename message_type, typename... parameters>
        status_code enqueue (parameters&&... args)
        {
            static_assert (std::is_base_of<message, message_type>::value,
			   "Invalid message_type - should be derived from mqmx::message");
            return push (new_message<message_type> (std::forward<parameters> (args)...));
        }

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
