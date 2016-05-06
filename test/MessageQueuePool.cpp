#include <mqmx/MessageQueuePool.h>
#include <gmock/gmock.h>

TEST (message_queue_pool, sanity_checks)
{
    mqmx::MessageQueuePool sut;
    ASSERT_TRUE (sut.isPollIdle ());

    auto mq = sut.allocateQueue (mqmx::MessageQueuePool::message_handler_func_type ());
    ASSERT_EQ (nullptr, mq.get ());

    mq = sut.allocateQueue ([](mqmx::Message::upointer_type && msg)->mqmx::status_code
                            {
                                return mqmx::ExitStatus::Success;
                            });
    ASSERT_NE (nullptr, mq.get ());
}
