#include "test/mocks/MessageQueueListener.h"

TEST (message_queue, move_ctor)
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

    msg = queueB.pop (); /* message should be moved from original queue */
    ASSERT_NE (nullptr, msg.get ());
    ASSERT_EQ (defQID, msg->getQID ());
    ASSERT_EQ (defMID, msg->getMID ());
}
