#include <mqmx/message_queue_poll.h>
#include <cassert>
#include <algorithm>

namespace mqmx
{
    message_queue_poll::message_queue_poll ()
        : _poll_mutex ()
	, _notifications_mutex ()
	, _notifications_condition ()
	, _notifications ()
    {
    }

    message_queue_poll::~message_queue_poll ()
    {
    }

    void message_queue_poll::notify (const queue_id_type qid,
				     message_queue * mq) noexcept
    {
	try
	{
	    message_queue::lock_type notifications_guard (_notifications_mutex);
	    const auto compare = [](const notification_rec & a,
				    const notification_rec & b) {
		return a.first < b.first;
	    };
	    const notification_rec elem (qid, mq);
	    notifications_list::const_iterator iter = std::upper_bound (
		_notifications.begin (), _notifications.end (), elem, compare);
	    if (iter != _notifications.begin ())
	    {
		notifications_list::const_iterator prev = iter;
		if ((--prev)->first == qid)
		{
		    return; /* queue already has some notification(s) */
		}
	    }
	    _notifications.insert (iter, elem);
	}
	catch (...)
	{ }
    }

    message_queue_poll::notifications_list message_queue_poll::poll (
	const std::vector<message_queue *> & mqs,
        const wait_time_provider & wtp)
    {
	lock_type poll_guard (_poll_mutex); /* to block re-entrance */
	{
	    lock_type notifications_guard (_notifications_mutex);
	    _notifications.clear ();
	}

	for (const auto & mq : mqs)
	{
	    const status_code ret_code = mq->set_listener (*this);
	    assert (ret_code == ExitStatus::Success);
	}

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
		_notifications_condition.wait_until (notifications_guard, abs_time, pred);
	    }
	}
	return _notifications;
    }
} /* namespace mqmx */
