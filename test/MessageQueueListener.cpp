#include "test/mocks/MessageQueueListener.h"

TEST (message_queue, NewData_and_Closed_notifications)
{
    using namespace mqmx;
    using namespace ::testing;
    const queue_id_type defQID = 10;
    const message_id_type defMID = 10;

    mocks::ListenerMock mock;

    EXPECT_CALL (mock, notify (defQID, nullptr, MessageQueue::NotificationFlag::Closed))
        .Times (1);
    {
        mqmx::MessageQueue queue (defQID);
        mqmx::Message::upointer_type msg;

        status_code retCode = queue.setListener (mock);
        ASSERT_EQ (ExitStatus::Success, retCode);

        /* test for double insert */
        retCode = queue.setListener (mock);
        ASSERT_EQ (ExitStatus::AlreadyExist, retCode);

        EXPECT_CALL (mock, notify (defQID, &queue, MessageQueue::NotificationFlag::NewData))
            .Times (1);

        retCode = queue.enqueue<mqmx::Message> (defMID);
        ASSERT_EQ (ExitStatus::Success, retCode);
    }
}

TEST (message_queue, Detached_because_of_move_ctor)
{
    using namespace mqmx;
    using namespace ::testing;
    const queue_id_type defQID = 10;
    const message_id_type defMID = 10;

    mocks::ListenerMock mock;
    mqmx::MessageQueue queue (defQID);
    mqmx::Message::upointer_type msg;

    status_code retCode = queue.setListener (mock);
    ASSERT_EQ (ExitStatus::Success, retCode);

    EXPECT_CALL (mock, notify (defQID, &queue, MessageQueue::NotificationFlag::NewData))
	.Times (1);
    retCode = queue.enqueue<mqmx::Message> (defMID);
    ASSERT_EQ (ExitStatus::Success, retCode);

    EXPECT_CALL (mock, notify (defQID, &queue, MessageQueue::NotificationFlag::Detached))
	.Times (1);
    MessageQueue queueB (std::move (queue));

    msg = queue.pop (); /* message should be moved to the new queue */
    ASSERT_EQ (nullptr, msg.get ());

    msg = queueB.pop (); /* message should be moved from the original queue */
    ASSERT_NE (nullptr, msg.get ());
    ASSERT_EQ (defQID, msg->getQID ());
    ASSERT_EQ (defMID, msg->getMID ());
}

TEST (message_queue, Detached_because_of_move_assignment)
{
    using namespace mqmx;
    using namespace ::testing;
    const queue_id_type defQIDa = 10;
    const queue_id_type defQIDb = 20;
    const message_id_type defMIDa = 10;
    const message_id_type defMIDb = 20;

    mocks::ListenerMock mock;

    MessageQueue queueA (defQIDa);
    MessageQueue queueB (defQIDb);

    mqmx::Message::upointer_type msg;
    status_code retCode = ExitStatus::Success;

    retCode = queueA.setListener (mock);
    ASSERT_EQ (ExitStatus::Success, retCode);

    retCode = queueB.setListener (mock);
    ASSERT_EQ (ExitStatus::Success, retCode);

    EXPECT_CALL (mock, notify (defQIDa, &queueA, MessageQueue::NotificationFlag::NewData))
	.Times (1);
    EXPECT_CALL (mock, notify (defQIDb, &queueB, MessageQueue::NotificationFlag::NewData))
	.Times (1);
    retCode = queueA.enqueue<mqmx::Message> (defMIDa);
    retCode = queueB.enqueue<mqmx::Message> (defMIDb);

    EXPECT_CALL (mock, notify (defQIDa, &queueA, MessageQueue::NotificationFlag::Detached))
	.Times (1);
    EXPECT_CALL (mock, notify (defQIDb, &queueB, MessageQueue::NotificationFlag::Detached))
	.Times (1);
    queueB = std::move (queueA);

    msg = queueA.pop (); /* message should be moved to the new queue */
    ASSERT_EQ (nullptr, msg.get ());

    msg = queueB.pop (); /* message should be moved from the original queue */
    ASSERT_NE (nullptr, msg.get ());
    ASSERT_EQ (defQIDa, msg->getQID ());
    ASSERT_EQ (defMIDa, msg->getMID ());
}
