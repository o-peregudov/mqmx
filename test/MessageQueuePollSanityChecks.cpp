#include "test/fixtures/MessageQueuePoll.h"
#include <gmock/gmock.h>

struct MQPollFixture : ::testing::Test
		     , fixtures::MessageQueuePoll
{
    mqmx::MessageQueuePoll sut;
};

TEST_F (MQPollFixture, sanity_checks)
{
    auto mqlist = sut.poll (std::begin (mq), std::end (mq));
    ASSERT_TRUE (mqlist.empty ());
}

TEST_F (MQPollFixture, initial_notification)
{
    using namespace mqmx;
    const size_t STRIDE = 3;
    size_t nqueues_signaled = 0;
    for (size_t ix = 0; ix < NQUEUES; ix += STRIDE)
    {
	mq[ix]->push (mq[ix]->newMessage<Message> (0));
	++nqueues_signaled;
    }

    auto mqlist = sut.poll (std::begin (mq), std::end (mq));
    ASSERT_EQ (nqueues_signaled, mqlist.size ());
}
