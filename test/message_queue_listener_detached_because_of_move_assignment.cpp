#include "mqmx/message_queue.h"
#include "test/FakeIt/single_header/standalone/fakeit.hpp"

#undef NDEBUG
#include <cassert>

int main (int argc, const char ** argv)
{
    using namespace fakeit;
    const mqmx::queue_id_type defQIDa = 10;
    const mqmx::queue_id_type defQIDb = 20;
    const mqmx::message_id_type defMIDa = 10;
    const mqmx::message_id_type defMIDb = 20;

    Mock<mqmx::message_queue::listener> mock;
    Fake (Method (mock, notify));

    mqmx::message_queue aqueue (defQIDa);
    mqmx::message_queue bqueue (defQIDb);

    mqmx::message::upointer_type msg;
    mqmx::status_code retCode = mqmx::ExitStatus::Success;

    retCode = aqueue.set_listener (mock.get ());
    assert (mqmx::ExitStatus::Success == retCode);

    retCode = bqueue.set_listener (mock.get ());
    assert (mqmx::ExitStatus::Success == retCode);

    retCode = aqueue.enqueue<mqmx::message> (defMIDa);
    retCode = bqueue.enqueue<mqmx::message> (defMIDb);

    Verify (Method (mock, notify).Using (
                defQIDa, &aqueue, mqmx::message_queue::notification_flag::data))
        .Once ();
    Verify (Method (mock, notify).Using (
                defQIDb, &bqueue, mqmx::message_queue::notification_flag::data))
        .Once ();
    bqueue = std::move (aqueue);

    Verify (Method (mock, notify).Using (
                defQIDa, &aqueue, mqmx::message_queue::notification_flag::detached))
        .Once ();
    Verify (Method (mock, notify).Using (
                defQIDb, &bqueue, mqmx::message_queue::notification_flag::detached))
        .Once ();

    msg = aqueue.pop (); /* message should be moved to the new queue */
    assert (nullptr == msg.get ());

    msg = bqueue.pop (); /* message should be moved from the original queue */
    assert (nullptr != msg.get ());
    assert (defQIDa == msg->get_qid ());
    assert (defMIDa == msg->get_mid ());

    VerifyNoOtherInvocations (mock);
    return 0;
}
