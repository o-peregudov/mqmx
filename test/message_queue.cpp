#include "mqmx/message_queue.h"
#include <cassert>

int main (int argc, const char ** argv)
{
    using namespace mqmx;
    message_queue queue (10);

    message_queue::message_ptr_type msg (queue.pop ());
    assert ((msg.get () == nullptr) &&
	    ("Initially queue is empty!"));

    int retCode = queue.push (message_queue::message_ptr_type (new message (10, 10)));
    assert ((retCode == ExitStatus::Success) &&
	    ("Push should succeed!"));

    message_queue queue2 (std::move (queue));

    msg = queue.pop ();
    assert ((msg.get () == nullptr) &&
	    ("Should be empty after move!"));

    retCode = queue.push (message_queue::message_ptr_type (new message (10, 10)));
    assert ((retCode == ExitStatus::NotSupported) &&
	    ("Push should fail, because moved out!"));

    msg = queue2.pop ();
    assert ((msg.get () != nullptr) &&
	    ("Should not be empty, because moved from existing!"));
    assert ((msg->get_qid () == 10) &&
	    ("QID should match!"));
    assert ((msg->get_mid () == 10) &&
	    ("MID should match!"));

    queue = std::move (queue2);
    retCode = queue.push (message_queue::message_ptr_type (new message (10, 10)));
    assert ((retCode == ExitStatus::Success) &&
	    ("Push should succeed, because moved in!"));
    retCode = queue2.push (message_queue::message_ptr_type (new message (10, 10)));
    assert ((retCode == ExitStatus::NotSupported) &&
	    ("Push should fail, because moved out!"));

    return 0;
}
