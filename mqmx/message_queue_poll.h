#ifndef MQMX_MESSAGE_QUEUE_POLL_H_INCLUDED
#define MQMX_MESSAGE_QUEUE_POLL_H_INCLUDED 1

#include <mqmx/message_queue.h>
#include <mqmx/wait_time_provider.h>

#include <condition_variable>
#include <vector>

namespace mqmx
{
    class message_queue_poll : message_queue::listener
    {
        message_queue_poll (const message_queue_poll &) = delete;
        message_queue_poll & operator = (const message_queue_poll &) = delete;

    public:
        typedef message_queue::mutex_type     mutex_type;
        typedef message_queue::lock_type      lock_type;
	typedef std::condition_variable       condvar_type;
        typedef std::pair<queue_id_type, message_queue *>
                                              notification_rec;
        typedef std::vector<notification_rec> notifications_list;

    private:
	mutex_type         _poll_mutex;
	mutable mutex_type _notifications_mutex;
	condvar_type       _notifications_condition;
        notifications_list _notifications;

        virtual void notify (const queue_id_type, message_queue *) noexcept override;

    public:
        message_queue_poll ();
        virtual ~message_queue_poll ();

	notifications_list poll (const std::vector<message_queue *> &,
				 const wait_time_provider & = wait_time_provider ());
    };
} /* namespace mqmx */
#endif /* MQMX_MESSAGE_QUEUE_POLL_H_INCLUDED */
