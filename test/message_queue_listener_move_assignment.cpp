#include "mqmx/message_queue.h"
#include "test/stub/listener.h"

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
    stub::listener slistener;

    {
	/*
	 * default constructor
	 */
	message_queue queueA (defQIDa);
	message_queue::message_ptr_type msg (queueA.pop ());
	assert ((msg.get () == nullptr) &&
		("Initially queue is empty"));

	status_code retCode = queueA.set_listener (slistener);
	assert ((retCode == ExitStatus::Success) &&
		("Listener should be registered"));

	message_queue queueB (defQIDb);
	msg = queueB.pop ();
	assert ((msg.get () == nullptr) &&
		("Initially queue is empty"));

	retCode = queueB.set_listener (slistener);
	assert ((retCode == ExitStatus::Success) &&
		("Listener should be registered"));

	/*
	 * push sample data
	 */
	retCode = queueA.push (
	    message_queue::message_ptr_type (new message (defQIDa, defMID)));
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
	assert ((msg->get_qid () == defQIDa) &&
		("There should be a message from QID A"));

	assert ((slistener.get_notifications ().size () == 2) &&
		("Both queues should be 'Detached'"));
	assert (((std::get<2> (slistener.get_notifications ()[0]) &
		  message_queue::NotificationFlag::Detached) != 0) &&
		("'Detached' flag should be reported"));
	assert (((std::get<2> (slistener.get_notifications ()[1]) &
		  message_queue::NotificationFlag::Detached) != 0) &&
		("'Detached' flag should be reported"));

	slistener.clear_notifications ();
    }
    assert ((slistener.get_notifications ().size () == 0) &&
	    ("Both queues should be 'Detached', so no 'Closed' notification"));
    return 0;
}
