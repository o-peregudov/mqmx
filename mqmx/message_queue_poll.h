#ifndef MQMX_MESSAGE_QUEUE_POLL_H_INCLUDED
#define MQMX_MESSAGE_QUEUE_POLL_H_INCLUDED 1

#include <mqmx/message_queue.h>
#include <mqmx/wait_time_provider.h>

#include <condition_variable>
#include <algorithm>
#include <vector>
#include <cassert>
#include <iterator>

namespace mqmx
{
    class message_queue_poll final : message_queue::listener
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

	/*
	 * NOTE: iterators should belong to a sequence of pointers to message_queue
	 */
	template <typename ForwardIt>
        notifications_list poll (const ForwardIt it_begin, const ForwardIt it_end,
				 const wait_time_provider & wtp = wait_time_provider ())
	{
	    lock_type poll_guard (_poll_mutex); /* to block re-entrance */
	    {
		lock_type notifications_guard (_notifications_mutex);
		_notifications.clear ();
	    }

	    std::for_each (it_begin, it_end,
			   [&](typename std::iterator_traits<ForwardIt>::reference mq)
			   {
			       const status_code ret_code = mq->set_listener (*this);
			       assert (ret_code == ExitStatus::Success);
			   });

	    lock_type notifications_guard (_notifications_mutex);
	    const auto abs_time = wtp.get_time_point ();
	    if (_notifications.empty ())
	    {
		const auto pred = [&]{ return !_notifications.empty (); };
		if (wtp.wait_infinitely ())
		{
		    _notifications_condition.wait (notifications_guard, pred);
		}
		else if (abs_time.time_since_epoch ().count () != 0)
		{
		    _notifications_condition.wait_until (
			notifications_guard, abs_time, pred);
		}
	    }
	    return _notifications;
	}
    };
} /* namespace mqmx */
#endif /* MQMX_MESSAGE_QUEUE_POLL_H_INCLUDED */