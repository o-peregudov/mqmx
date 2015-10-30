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
} /* namespace mqmx */
