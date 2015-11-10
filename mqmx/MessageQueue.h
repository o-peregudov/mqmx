#pragma once

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
	typedef std::unique_ptr<MessageQueue>      upointer_type;
        typedef std::mutex                         mutex_type;
        typedef std::unique_lock<mutex_type>       lock_type;
        typedef std::deque<Message::upointer_type> container_type;
        typedef size_t                             notification_flags_type;

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

        status_code push (Message::upointer_type && msg);
	Message::upointer_type pop ();

	template <typename MessageType, typename... ParametersTypes>
	Message::upointer_type newMessage (ParametersTypes&&... args) const
	{
	    return Message::upointer_type (
		new MessageType (getQID (), std::forward<ParametersTypes> (args)...));
	}
	
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
