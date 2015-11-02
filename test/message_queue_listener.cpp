#include "mqmx/message_queue.h"
#include <cassert>
#include <vector>
#include <algorithm>

class listener_mock : public mqmx::message_queue::listener
{
public:
    typedef std::tuple<mqmx::queue_id_type,
		       mqmx::message_queue *,
		       mqmx::message_queue::notification_flags_type> notification_rec;
    typedef std::vector<notification_rec>                            notification_list;

private:
    notification_list _notifications;

public:
    listener_mock ()
	: mqmx::message_queue::listener ()
	, _notifications ()
    { }

    virtual ~listener_mock ()
    { }

    virtual void notify (
	const mqmx::queue_id_type qid,
	mqmx::message_queue * mq,
	const mqmx::message_queue::notification_flags_type flag) noexcept override
    {
	try
	{
	    const notification_rec elem (qid, mq, flag);
	    const auto compare = [](const notification_rec & a,
				    const notification_rec & b) {
		return std::get<0> (a) < std::get<0> (b);
	    };

	    notification_list::iterator iter = std::upper_bound (
		_notifications.begin (), _notifications.end (), elem, compare);
	    if (iter != _notifications.begin ())
	    {
		notification_list::iterator prev = iter;
		if (std::get<0> (*(--prev)) == qid)
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

int main (int argc, const char ** argv)
{
    using namespace mqmx;
    const queue_id_type defQID = 10;
    const message_id_type defMID = 10;

    /*
     * listener should have longer lifetime than message_queue
     */
    listener_mock sample_listener;
    {
	/*
	 * default constructor
	 */
	message_queue queue (defQID);
	message_queue::message_ptr_type msg (queue.pop ());
	assert ((msg.get () == nullptr) &&
		("Initially queue is empty"));

	status_code retCode = queue.set_listener (sample_listener);
	assert ((retCode == ExitStatus::Success) &&
		("Listener should be registered"));

	/*
	 * push operation
	 */
	retCode = queue.push (message_queue::message_ptr_type (new message (defQID, defMID)));
	assert ((retCode == ExitStatus::Success) &&
		("Push should succeed"));
	assert ((sample_listener.get_notifications ().size () == 1) &&
		("Single notification should be delivered"));
	assert (((std::get<2> (sample_listener.get_notifications ().front ()) &
		  message_queue::NotificationFlag::NewData) != 0) &&
		("Proper notification flag should be delivered"));
    }
    return 0;
}
