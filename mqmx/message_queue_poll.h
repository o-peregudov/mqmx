#ifndef MQMX_MESSAGE_QUEUE_POLL_H_INCLUDED
#define MQMX_MESSAGE_QUEUE_POLL_H_INCLUDED 1

#include <mqmx/message_queue.h>
#include <mqmx/wait_time_provider.h>

#include <condition_variable>
#include <algorithm>
#include <vector>
#include <cassert>
#include <iterator>
#include <tuple>

namespace mqmx
{
    class message_queue_poll final : message_queue::listener
    {
        message_queue_poll (const message_queue_poll &) = delete;
        message_queue_poll & operator = (const message_queue_poll &) = delete;

    public:
        typedef message_queue::mutex_type                 mutex_type;
        typedef message_queue::lock_type                  lock_type;
	typedef std::condition_variable                   condvar_type;
        typedef std::tuple<queue_id_type, message_queue *,
			   message_queue::notification_flags_type>
	                                                  notification_rec;
        typedef std::vector<notification_rec>             notifications_list;

    private:
	mutex_type         _poll_mutex;
	mutable mutex_type _notifications_mutex;
	condvar_type       _notifications_condition;
        notifications_list _notifications;

        virtual void notify (const queue_id_type,
			     message_queue *,
			     const message_queue::notification_flags_type) noexcept override;

    public:
        message_queue_poll ();
        virtual ~message_queue_poll ();

	/*
	 * NOTE: iterators should represent a sequence of pointers to message_queue
	 */
	template <typename forward_it,
		  typename ref_clock_provider = WaitTimeProvider>
        notifications_list poll (const forward_it ibegin, const forward_it iend,
				 const WaitTimeProvider & wtp = WaitTimeProvider (),
				 const ref_clock_provider & rcp = WaitTimeProvider ())
	{
	    lock_type poll_guard (_poll_mutex); /* to block re-entrance */
	    {
		/*
		 * initialize list of notifications
		 */
		lock_type notifications_guard (_notifications_mutex);
		_notifications.clear ();
	    }

	    /*
	     * set listeners for each message queue
	     */
	    std::for_each (ibegin, iend,
			   [&](typename std::iterator_traits<forward_it>::reference mq)
			   {
			       const status_code ret_code = mq->set_listener (*this);
			       assert (ret_code == ExitStatus::Success);
			   });

	    /*
	     * wait for notifications
	     */
	    lock_type notifications_guard (_notifications_mutex);
	    const auto abs_time = wtp.getTimepoint (rcp);
	    if (_notifications.empty ())
	    {
		const auto pred = [&]{ return !_notifications.empty (); };
		if (wtp.waitInfinitely ())
		{
		    _notifications_condition.wait (notifications_guard, pred);
		}
		else if (abs_time.time_since_epoch ().count () != 0)
		{
		    _notifications_condition.wait_until (
			notifications_guard, abs_time, pred);
		}
	    }

	    /*
	     * remove listeners from each message queue
	     */
	    std::for_each (ibegin, iend,
			   [&](typename std::iterator_traits<forward_it>::reference mq)
			   {
			       mq->clear_listener ();
			   });
	    return _notifications;
	}
    };
} /* namespace mqmx */
#endif /* MQMX_MESSAGE_QUEUE_POLL_H_INCLUDED */
