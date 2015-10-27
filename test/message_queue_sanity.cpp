#include "mqmx/message_queue.h"
#include <cassert>

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
            ("Initially queue is empty!"));

    /*
     * push operation
     */
    status_code retCode = queue.push (
        message_queue::message_ptr_type (new message (defQID, defMID)));
    assert ((retCode == ExitStatus::Success) &&
            ("Push should succeed!"));

    /*
     * move constructor
     */
    message_queue queue2 (std::move (queue));

    msg = queue.pop ();
    assert ((msg.get () == nullptr) &&
            ("Should be empty after move!"));

    retCode = queue.push (
        message_queue::message_ptr_type (new message (defQID, defMID)));
    assert ((retCode == ExitStatus::NotSupported) &&
            ("Push should fail, because moved out!"));

    /*
     * queue should be moved after move constructor
     */
    msg = queue2.pop ();
    assert ((msg.get () != nullptr) &&
            ("Should not be empty, because moved from existing!"));
    assert ((msg->get_qid () == defQID) &&
            ("QID should match!"));
    assert ((msg->get_mid () == defMID) &&
            ("MID should match!"));

    /*
     * move assignment
     */
    queue = message_queue (defQID);
    msg = queue.pop ();
    assert ((msg.get () == nullptr) &&
            ("Initially queue is empty!"));

    /*
     * FIFO message ordering
     */
    for (size_t ix = 0; ix < 10; ++ix)
    {
        retCode = queue.push (
            message_queue::message_ptr_type (new message (defQID, defMID + ix)));
        assert ((retCode == ExitStatus::Success) &&
                ("Push should succeed!"));
    }
    for (size_t ix = 0; ix < 10; ++ix)
    {
        msg = queue.pop ();
        assert ((msg.get () != nullptr) &&
                ("Should not be empty, because moved from existing!"));
        assert ((msg->get_qid () == defQID) &&
                ("QID should match!"));
        assert ((msg->get_mid () == (defMID + ix)) &&
                ("MID should match!"));
    }
    return 0;
}
