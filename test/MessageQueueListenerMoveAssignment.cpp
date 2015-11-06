#include "test/stubs/listener.h"

#include <cassert>

int main (int argc, const char ** argv)
{
    using namespace mqmx;
    const queue_id_type defQIDa = 10;
    const queue_id_type defQIDb = 20;
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
	MessageQueue queueA (defQIDa);
	Message::upointer_type msg (queueA.pop ());
	assert ((msg.get () == nullptr) &&
		("Initially queue is empty"));

	status_code retCode = queueA.setListener (slistener);
	assert ((retCode == ExitStatus::Success) &&
		("Listener should be registered"));

	MessageQueue queueB (defQIDb);
	msg = queueB.pop ();
	assert ((msg.get () == nullptr) &&
		("Initially queue is empty"));

	retCode = queueB.setListener (slistener);
	assert ((retCode == ExitStatus::Success) &&
		("Listener should be registered"));

	/*
	 * push sample data
	 */
	retCode = queueA.push (Message::upointer_type (new Message (defQIDa, defMID)));
	slistener.clear_notifications ();

	/*
	 * move queue with assignment
	 */
	queueB = std::move (queueA);

	msg = queueA.pop ();
	assert ((msg.get () == nullptr) &&
		("Queue A should be empty, because moved out"));

	msg = queueB.pop ();
	assert ((msg.get () != nullptr) &&
		("Queue A should not be empty, because moved in"));
	assert ((msg->getQID () == defQIDa) &&
		("There should be a message from QID A"));

	assert ((slistener.get_notifications ().size () == 2) &&
		("Both queues should be 'Detached'"));
	assert (((std::get<2> (slistener.get_notifications ()[0]) &
		  MessageQueue::NotificationFlag::Detached) != 0) &&
		("'Detached' flag should be reported"));
	assert (((std::get<2> (slistener.get_notifications ()[1]) &
		  MessageQueue::NotificationFlag::Detached) != 0) &&
		("'Detached' flag should be reported"));

	slistener.clear_notifications ();
    }
    assert ((slistener.get_notifications ().size () == 0) &&
	    ("Both queues should be 'Detached', so no 'Closed' notification"));
    return 0;
}
