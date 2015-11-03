#include <mqmx/message_queue_poll.h>

namespace mqmx
{
    MessageQueuePoll::MessageQueuePoll ()
        : _poll_mutex ()
	, _notifications_mutex ()
	, _notifications_condition ()
	, _notifications ()
    {
    }

    MessageQueuePoll::~MessageQueuePoll ()
    {
    }

    void MessageQueuePoll::notify (
	const queue_id_type qid,
	MessageQueue * mq,
	const MessageQueue::notification_flags_type flag) noexcept
    {
	try
	{
	    const notification_rec_type elem (qid, mq, flag);
	    const auto compare = [](const notification_rec_type & a,
				    const notification_rec_type & b) {
		return ((std::get<0> (a) <= std::get<0> (b)) &&
			(std::get<1> (a) <  std::get<1> (b)));
	    };

	    MessageQueue::lock_type notifications_guard (_notifications_mutex);
	    notifications_list_type::iterator iter = std::upper_bound (
		_notifications.begin (), _notifications.end (), elem, compare);
	    if (iter != _notifications.begin ())
	    {
		notifications_list_type::iterator prev = iter;
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
