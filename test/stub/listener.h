#pragma once

#include <mqmx/MessageQueue.h>

#include <tuple>
#include <vector>
#include <algorithm>

namespace stub
{
    class listener : public mqmx::MessageQueue::Listener
    {
    public:
	typedef std::tuple<mqmx::queue_id_type,
			   mqmx::MessageQueue *,
			   mqmx::MessageQueue::notification_flags_type> notification_rec;
	typedef std::vector<notification_rec>                           notification_list;

    private:
	notification_list _notifications;

    public:
	listener ()
	    : mqmx::MessageQueue::Listener ()
	    , _notifications ()
	{ }

	virtual ~listener ()
	{ }

	virtual void notify (
	    const mqmx::queue_id_type qid,
	    mqmx::MessageQueue * mq,
	    const mqmx::MessageQueue::notification_flags_type flag) noexcept override
	{
	    try
	    {
		const notification_rec elem (qid, mq, flag);
		const auto compare = [](const notification_rec & a,
					const notification_rec & b) {
		    return ((std::get<0> (a) <= std::get<0> (b)) &&
			    (std::get<1> (a) <  std::get<1> (b)));
		};

		notification_list::iterator iter = std::upper_bound (
		    _notifications.begin (), _notifications.end (), elem, compare);
		if (iter != _notifications.begin ())
		{
		    notification_list::iterator prev = iter;
		    --prev;

		    if ((std::get<0> (*prev) == qid) && (std::get<1> (*prev) == mq))
		    {
			std::get<2> (*prev) |= flag;
			return; /* queue already has some notification(s) */
		    }
		}
		_notifications.insert (iter, elem);
	    }
	    catch (...)
	    { }
	}

	notification_list get_notifications () const
	{
	    return _notifications;
	}

	void clear_notifications ()
	{
	    _notifications.clear ();
	}
    };
} /* namespace stub */
