#include "test/stubs/listener.h"

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
    stubs::listener slistener;

    {
	/*
	 * default constructor
	 */
	MessageQueue queueA (defQID);
	MessageQueue::message_ptr_type msg (queueA.pop ());
	assert ((msg.get () == nullptr) &&
		("Initially queue is empty"));

	status_code retCode = queueA.setListener (slistener);
	assert ((retCode == ExitStatus::Success) &&
		("Listener should be registered"));

	/*
	 * push sample data and move queue
	 */
	retCode = queueA.push (
	    MessageQueue::message_ptr_type (new Message (defQID, defMID)));
	MessageQueue queueB (std::move (queueA));

	msg = queueA.pop ();
        assert ((msg.get () == nullptr) &&
		("Original queue should be moved out"));

	msg = queueB.pop ();
        assert ((msg.get () != nullptr) &&
		("New queue should not be empty"));
        assert ((msg->getQID () == defQID) &&
		("QID should match"));
        assert ((msg->getMID () == defMID) &&
		("MID should match"));

	assert ((slistener.get_notifications ().size () == 1) &&
		("Single notification should be delivered"));
	assert (((std::get<2> (slistener.get_notifications ().front ()) &
		  MessageQueue::NotificationFlag::Detached) != 0) &&
		("'Detached' notification flag should be delivered"));
	assert ((std::get<1> (slistener.get_notifications ().front ()) == &queueA) &&
		("'Detached' notification flag should be delivered for original queue"));
    }

    /*
     * MQ destructor should deliver "Closed" notification
     */
    assert ((slistener.get_notifications ().size () == 1) &&
	    ("No extra notifications should be delivered"));
    assert (((std::get<2> (slistener.get_notifications ().front ()) &
	      MessageQueue::NotificationFlag::Closed) == 0) &&
	     ("No 'Closed' flag should be reported, because queue is 'Detached'"));

    return 0;
}
