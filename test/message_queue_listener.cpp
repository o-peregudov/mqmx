#include "mqmx/message_queue.h"
#include "test/FakeIt/single_header/standalone/fakeit.hpp"

#undef NDEBUG
#include <cassert>

int main (int argc, const char ** argv)
{
    {
        /*
         * 'detached' because of move ctor
         */
        using namespace fakeit;
        const mqmx::queue_id_type defQID = 10;
        const mqmx::message_id_type defMID = 10;

        Mock<mqmx::message_queue::listener> mock;
        Fake (Method (mock, notify));

        mqmx::message_queue queue (defQID);
        mqmx::message::upointer_type msg;

        mqmx::status_code retCode = queue.set_listener (mock.get ());
        assert (mqmx::ExitStatus::Success == retCode);

        retCode = queue.enqueue<mqmx::message> (defMID);
        assert (mqmx::ExitStatus::Success == retCode);

        Verify (Method (mock, notify).Using (
                    defQID, &queue, mqmx::message_queue::notification_flag::data))
            .Once ();

        mqmx::message_queue queueB (std::move (queue));
        Verify (Method (mock, notify).Using (
                    defQID, &queue, mqmx::message_queue::notification_flag::detached))
            .Once ();

        msg = queue.pop (); /* message should be moved to the new queue */
        assert (nullptr == msg.get ());

        msg = queueB.pop (); /* message should be moved from the original queue */
        assert (nullptr != msg.get ());
        assert (defQID == msg->get_qid ());
        assert (defMID == msg->get_mid ());

        VerifyNoOtherInvocations (mock);
    }
    {
        /*
         * 'detached' because of move assignment
         */
        using namespace fakeit;
        const mqmx::queue_id_type defQIDa = 10;
        const mqmx::queue_id_type defQIDb = 20;
        const mqmx::message_id_type defMIDa = 10;
        const mqmx::message_id_type defMIDb = 20;

        Mock<mqmx::message_queue::listener> mock;
	Fake (Method (mock, notify));

        mqmx::message_queue queueA (defQIDa);
        mqmx::message_queue queueB (defQIDb);

        mqmx::message::upointer_type msg;
        mqmx::status_code retCode = mqmx::ExitStatus::Success;

        retCode = queueA.set_listener (mock.get ());
        assert (mqmx::ExitStatus::Success == retCode);

        retCode = queueB.set_listener (mock.get ());
        assert (mqmx::ExitStatus::Success == retCode);

        retCode = queueA.enqueue<mqmx::message> (defMIDa);
        retCode = queueB.enqueue<mqmx::message> (defMIDb);

        Verify (Method (mock, notify).Using (
                    defQIDa, &queueA, mqmx::message_queue::notification_flag::data))
            .Once ();
        Verify (Method (mock, notify).Using (
                    defQIDb, &queueB, mqmx::message_queue::notification_flag::data))
            .Once ();
        queueB = std::move (queueA);

        Verify (Method (mock, notify).Using (
                    defQIDa, &queueA, mqmx::message_queue::notification_flag::detached))
            .Once ();
        Verify (Method (mock, notify).Using (
                    defQIDb, &queueB, mqmx::message_queue::notification_flag::detached))
            .Once ();

        msg = queueA.pop (); /* message should be moved to the new queue */
        assert (nullptr == msg.get ());

        msg = queueB.pop (); /* message should be moved from the original queue */
        assert (nullptr != msg.get ());
        assert (defQIDa == msg->get_qid ());
        assert (defMIDa == msg->get_mid ());

        VerifyNoOtherInvocations (mock);
    }
    return 0;
}
