#include "mqmx/message_queue.h"
#include "test/stub/listener.h"

#include <cassert>

int main (int argc, const char ** argv)
{
    using namespace mqmx;
    const queue_id_type defQID = 10;
    const message_id_type defMID = 10;

    /*
     * listener should have longer lifetime than message_queue
     * to check "Closed" notification from MQ
     */
    stub::listener slistener;

    {
	/*
	 * default constructor
	 */
	message_queue queue (defQID);
	message_queue::message_ptr_type msg (queue.pop ());
	assert ((msg.get () == nullptr) &&
		("Initially queue is empty"));

	status_code retCode = queue.set_listener (slistener);
	assert ((retCode == ExitStatus::Success) &&
		("Listener should be registered"));

	/*
	 * push operation
	 */
	retCode = queue.push (
	    message_queue::message_ptr_type (new message (defQID, defMID)));
	assert ((retCode == ExitStatus::Success) &&
		("Push should succeed"));
	assert ((slistener.get_notifications ().size () == 1) &&
		("Single notification should be delivered"));
	assert (((std::get<2> (slistener.get_notifications ().front ()) &
		  message_queue::NotificationFlag::NewData) != 0) &&
		("'NewData' notification flag should be delivered"));

	slistener.clear_notifications ();
    }

    /*
     * MQ destructor should deliver "Closed" notification
     */
    assert ((slistener.get_notifications ().size () == 1) &&
	    ("Still single notification should be delivered"));
    assert (((std::get<2> (slistener.get_notifications ().front ()) &
		       message_queue::NotificationFlag::Closed) != 0) &&
	     ("'Closed' notification flag should be delivered"));
    assert ((std::get<1> (slistener.get_notifications ().front ()) == nullptr) &&
            ("'Closed' notification should be delivered for nullptr"));

    return 0;
}
