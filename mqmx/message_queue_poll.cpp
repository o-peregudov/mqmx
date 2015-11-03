#include <mqmx/message_queue_poll.h>

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

    void message_queue_poll::notify (
	const queue_id_type qid,
	MessageQueue * mq,
	const MessageQueue::notification_flags_type flag) noexcept
    {
	try
	{
	    const notification_rec elem (qid, mq, flag);
	    const auto compare = [](const notification_rec & a,
				    const notification_rec & b) {
		return ((std::get<0> (a) <= std::get<0> (b)) &&
			(std::get<1> (a) <  std::get<1> (b)));
	    };

	    MessageQueue::lock_type notifications_guard (_notifications_mutex);
	    notifications_list::iterator iter = std::upper_bound (
		_notifications.begin (), _notifications.end (), elem, compare);
	    if (iter != _notifications.begin ())
	    {
		notifications_list::iterator prev = iter;
		--prev;

		if ((std::get<0> (*prev) == qid) && (std::get<1> (*prev) == mq))
		{
		    std::get<2> (*prev) |= flag;
		    _notifications_condition.notify_one ();
		    return; /* queue already has some notification(s) */
		}
	    }
	    _notifications.insert (iter, elem);
	    _notifications_condition.notify_one ();
	}
	catch (...)
	{ }
    }
} /* namespace mqmx */
