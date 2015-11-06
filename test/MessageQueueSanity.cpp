#include <mqmx/MessageQueue.h>
#include <cassert>

int main (int argc, const char ** argv)
{
    using namespace mqmx;
    const queue_id_type defQID = 10;
    const message_id_type defMID = 10;

    /*
     * default constructor
     */
    MessageQueue queue (defQID);
    Message::upointer_type msg (queue.pop ());
    assert ((msg.get () == nullptr) &&
            ("Initially queue is empty!"));

    /*
     * push operation
     */
    status_code retCode = queue.push (Message::upointer_type (new Message (defQID, defMID)));
    assert ((retCode == ExitStatus::Success) &&
            ("Push should succeed!"));

    /*
     * MQ exhaustion
     */
    msg = queue.pop ();
    assert ((msg.get () != nullptr) &&
	    ("Should not be empty!"));
    assert ((msg->getQID () == defQID) &&
	    ("QID should match!"));
    assert ((msg->getMID () == defMID) &&
	    ("MID should match!"));

    msg = queue.pop ();
    assert ((msg.get () == nullptr) &&
	    ("Should be empty!"));

    /*
     * FIFO message ordering
     */
    for (size_t ix = 0; ix < 10; ++ix)
    {
        retCode = queue.push (Message::upointer_type (new Message (defQID, defMID + ix)));
        assert ((retCode == ExitStatus::Success) &&
                ("Push should succeed!"));
    }
    for (size_t ix = 0; ix < 10; ++ix)
    {
        msg = queue.pop ();
        assert ((msg.get () != nullptr) &&
                ("Should not be empty!"));
        assert ((msg->getQID () == defQID) &&
                ("QID should match!"));
        assert ((msg->getMID () == (defMID + ix)) &&
                ("MID should match!"));
    }
    return 0;
}
