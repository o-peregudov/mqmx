#include "test/mocks/message_queue_listener.h"

TEST (message_queue, NewData_and_Closed_notifications)
{
    using namespace mqmx;
    using namespace ::testing;
    const queue_id_type defQID = 10;
    const message_id_type defMID = 10;

    mocks::message_queue_listener_mock mock;

    EXPECT_CALL (mock, notify (defQID, nullptr, message_queue::notification_flag::closed))
        .Times (1);
    {
        mqmx::message_queue queue (defQID);
        mqmx::message::upointer_type msg;

        status_code retCode = queue.set_listener (mock);
        ASSERT_EQ (ExitStatus::Success, retCode);

        /* test for double insert */
        retCode = queue.set_listener (mock);
        ASSERT_EQ (ExitStatus::AlreadyExist, retCode);

        EXPECT_CALL (mock, notify (defQID, &queue, message_queue::notification_flag::data))
            .Times (1);

        retCode = queue.enqueue<mqmx::message> (defMID);
        ASSERT_EQ (ExitStatus::Success, retCode);
    }
}

TEST (message_queue, Detached_because_of_move_ctor)
{
    using namespace mqmx;
    using namespace ::testing;
    const queue_id_type defQID = 10;
    const message_id_type defMID = 10;

    mocks::message_queue_listener_mock mock;
    mqmx::message_queue queue (defQID);
    mqmx::message::upointer_type msg;

    status_code retCode = queue.set_listener (mock);
    ASSERT_EQ (ExitStatus::Success, retCode);

    EXPECT_CALL (mock, notify (defQID, &queue, message_queue::notification_flag::data))
	.Times (1);
    retCode = queue.enqueue<mqmx::message> (defMID);
    ASSERT_EQ (ExitStatus::Success, retCode);

    EXPECT_CALL (mock, notify (defQID, &queue, message_queue::notification_flag::detached))
	.Times (1);
    message_queue queueB (std::move (queue));

    msg = queue.pop (); /* message should be moved to the new queue */
    ASSERT_EQ (nullptr, msg.get ());

    msg = queueB.pop (); /* message should be moved from the original queue */
    ASSERT_NE (nullptr, msg.get ());
    ASSERT_EQ (defQID, msg->get_qid ());
    ASSERT_EQ (defMID, msg->get_mid ());
}

TEST (message_queue, Detached_because_of_move_assignment)
{
    using namespace mqmx;
    using namespace ::testing;
    const queue_id_type defQIDa = 10;
    const queue_id_type defQIDb = 20;
    const message_id_type defMIDa = 10;
    const message_id_type defMIDb = 20;

    mocks::message_queue_listener_mock mock;

    message_queue queueA (defQIDa);
    message_queue queueB (defQIDb);

    mqmx::message::upointer_type msg;
    status_code retCode = ExitStatus::Success;

    retCode = queueA.set_listener (mock);
    ASSERT_EQ (ExitStatus::Success, retCode);

    retCode = queueB.set_listener (mock);
    ASSERT_EQ (ExitStatus::Success, retCode);

    EXPECT_CALL (mock, notify (defQIDa, &queueA, message_queue::notification_flag::data))
	.Times (1);
    EXPECT_CALL (mock, notify (defQIDb, &queueB, message_queue::notification_flag::data))
	.Times (1);
    retCode = queueA.enqueue<mqmx::message> (defMIDa);
    retCode = queueB.enqueue<mqmx::message> (defMIDb);

    EXPECT_CALL (mock, notify (defQIDa, &queueA, message_queue::notification_flag::detached))
	.Times (1);
    EXPECT_CALL (mock, notify (defQIDb, &queueB, message_queue::notification_flag::detached))
	.Times (1);
    queueB = std::move (queueA);

    msg = queueA.pop (); /* message should be moved to the new queue */
    ASSERT_EQ (nullptr, msg.get ());

    msg = queueB.pop (); /* message should be moved from the original queue */
    ASSERT_NE (nullptr, msg.get ());
    ASSERT_EQ (defQIDa, msg->get_qid ());
    ASSERT_EQ (defMIDa, msg->get_mid ());
}
