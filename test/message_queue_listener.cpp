#include "mqmx/message_queue.h"
#include <cassert>
#include <vector>
#include <algorithm>

class listener_mock : public mqmx::message_queue::listener
{
public:
    typedef std::pair<mqmx::queue_id_type,
		      mqmx::message_queue *> notification_rec;
    typedef std::vector<notification_rec>    notification_list;

private:
    notification_list _notifications;

public:
    listener_mock ()
	: mqmx::message_queue::listener ()
	, _notifications ()
    { }

    virtual ~listener_mock ()
    { }

    virtual void notify (const mqmx::queue_id_type qid,
			 mqmx::message_queue * mq) noexcept override
    {
	try
	{
	    const auto compare = [](const notification_rec & a,
				    const notification_rec & b) {
		return a.first < b.first;
	    };
	    const notification_rec elem (qid, mq);
	    notification_list::const_iterator iter = std::upper_bound (
		_notifications.begin (), _notifications.end (), elem, compare);
	    if (iter != _notifications.begin ())
	    {
		notification_list::const_iterator prev = iter;
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
     * default constructor
     */
    message_queue queue (defQID);
    message_queue::message_ptr_type msg (queue.pop ());
    assert ((msg.get () == nullptr) &&
            ("Initially queue is empty"));

    listener_mock sample_listener;
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
    return 0;
}
