#include "mqmx/message_queue.h"
#include "test/stub/listener.h"

#include <cassert>

void move_ctor_test ()
{
    using namespace mqmx;
    const queue_id_type defQID = 10;
    const message_id_type defMID = 10;

    /*
     * listener should have longer lifetime than message_queue
     * to check "Closed" notification from MQ
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
	 * push sample data
	 */
	retCode = queue.push (
	    message_queue::message_ptr_type (new message (defQID, defMID)));
	assert ((retCode == ExitStatus::Success) &&
		("Push should succeed"));
	assert ((sample_listener.get_notifications ().size () == 1) &&
		("Single notification should be delivered"));
	assert (((std::get<2> (sample_listener.get_notifications ().front ()) &
		  message_queue::NotificationFlag::NewData) != 0) &&
		("Proper notification flag should be delivered"));

	/*
	 * move queue
	 */
	message_queue queue2 (std::move (queue));

	msg = queue.pop ();
        assert ((msg.get () == nullptr) &&
		("Original queue should be moved out"));

	msg = queue2.pop ();
        assert ((msg.get () != nullptr) &&
		("New queue should not be empty"));
        assert ((msg->get_qid () == defQID) &&
		("QID should match"));
        assert ((msg->get_mid () == defMID) &&
		("MID should match"));

	assert ((sample_listener.get_notifications ().size () == 1) &&
		("Single notification should be delivered"));
	assert (((std::get<2> (sample_listener.get_notifications ().front ()) &
		  message_queue::NotificationFlag::Detached) != 0) &&
		("Proper notification flag should be delivered"));
    }

    /*
     * MQ destructor should deliver "Closed" notification
     */
    assert ((sample_listener.get_notifications ().size () == 1) &&
	    ("Still single notification should be delivered"));
    assert (((std::get<2> (sample_listener.get_notifications ().front ()) &
		       message_queue::NotificationFlag::Closed) == 0) &&
	     ("No 'Closed' flag should be reported, because queue is 'Detached'"));
}

void move_assignment_test ()
{
    using namespace mqmx;
    const queue_id_type defQID = 10;
    const message_id_type defMID = 10;

    /*
     * listener should have longer lifetime than message_queue
     * to check "Closed" notification from MQ
     */
    listener_mock sample_listener;

    {
	/*
	 * default constructor
	 */
	message_queue queueA (defQID);
	message_queue::message_ptr_type msg (queueA.pop ());
	assert ((msg.get () == nullptr) &&
		("Initially queue is empty"));

	status_code retCode = queueA.set_listener (sample_listener);
	assert ((retCode == ExitStatus::Success) &&
		("Listener should be registered"));

	message_queue queueB (defQID + 1);
	msg = queueB.pop ();
	assert ((msg.get () == nullptr) &&
		("Initially queue is empty"));

	retCode = queueB.set_listener (sample_listener);
	assert ((retCode == ExitStatus::Success) &&
		("Listener should be registered"));

	/*
	 * push sample data
	 */
	retCode = queueA.push (
	    message_queue::message_ptr_type (new message (defQID, defMID)));
	assert ((retCode == ExitStatus::Success) &&
		("Push should succeed"));
	assert ((sample_listener.get_notifications ().size () == 1) &&
		("Single notification should be delivered"));
	assert (((std::get<1> (sample_listener.get_notifications ().front ())->get_id ()) == defQID) &&
		("Proper notification flag should be delivered"));
	assert (((std::get<2> (sample_listener.get_notifications ().front ()) &
		  message_queue::NotificationFlag::NewData) != 0) &&
		("Proper notification flag should be delivered"));

	sample_listener.clear_notifications ();

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
	assert ((msg->get_qid () == defQID) &&
		("There should be a message from QID A"));

	assert ((sample_listener.get_notifications ().size () == 2) &&
		("Both queues should be 'Detached'"));
	assert (((std::get<2> (sample_listener.get_notifications ()[0]) &
		  message_queue::NotificationFlag::Detached) != 0) &&
		("'Detached' flag should be reported"));
	assert (((std::get<2> (sample_listener.get_notifications ()[1]) &
		  message_queue::NotificationFlag::Detached) != 0) &&
		("'Detached' flag should be reported"));
    }
}

int main (int argc, const char ** argv)
{
    basic_listener_test ();
    move_ctor_test ();
    move_assignment_test ();
    return 0;
}
