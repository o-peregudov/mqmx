#include "test/mocks/MessageQueueListener.h"

TEST (message_queue, default_listener_behavior)
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
