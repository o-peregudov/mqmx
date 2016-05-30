#include "mqmx/message_queue.h"
#include "test/FakeIt/single_header/standalone/fakeit.hpp"

#undef NDEBUG
#include <cassert>

int main (int argc, const char ** argv)
{
    using namespace fakeit;
    const mqmx::queue_id_type defQID = 10;
    const mqmx::message_id_type defMID = 10;

    Mock<mqmx::message_queue::listener> mock;
    Fake (Method (mock, notify));

    mqmx::message_queue aqueue (defQID);
    mqmx::message::upointer_type msg;

    mqmx::status_code retCode = aqueue.set_listener (mock.get ());
    assert (mqmx::ExitStatus::Success == retCode);

    retCode = aqueue.enqueue<mqmx::message> (defMID);
    assert (mqmx::ExitStatus::Success == retCode);

    Verify (Method (mock, notify).Using (
                defQID, &aqueue, mqmx::message_queue::notification_flag::data))
        .Once ();

    mqmx::message_queue bqueue (std::move (aqueue));
    Verify (Method (mock, notify).Using (
                defQID, &aqueue, mqmx::message_queue::notification_flag::detached))
        .Once ();

    msg = aqueue.pop (); /* message should be moved to the new queue */
    assert (nullptr == msg.get ());

    msg = bqueue.pop (); /* message should be moved from the original queue */
    assert (nullptr != msg.get ());
    assert (defQID == msg->get_qid ());
    assert (defMID == msg->get_mid ());

    VerifyNoOtherInvocations (mock);
    return 0;
}
