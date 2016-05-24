#include "test/fixtures/message_queue_poll.h"
#include <gmock/gmock.h>
#include <thread>

struct MQPollFixture : ::testing::Test
                     , fixtures::message_queue_poll_fixture
{
};

TEST_F (MQPollFixture, sanity_checks)
{
    auto mqlist = mqmx::poll (std::begin (mq), std::end (mq));
    ASSERT_TRUE (mqlist.empty ());
}

TEST_F (MQPollFixture, initial_notification)
{
    const size_t STRIDE = 3;
    size_t nqueues_signaled = 0;
    for (size_t ix = 0; ix < NQUEUES; ix += STRIDE)
    {
        mq[ix]->enqueue<mqmx::message> (0);
        ++nqueues_signaled;
    }

    auto mqlist = poll (std::begin (mq), std::end (mq));
    ASSERT_EQ (nqueues_signaled, mqlist.size ());
}

TEST_F (MQPollFixture, infinite_wait)
{
    const size_t idx = NQUEUES - 1;
    const mqmx::message_id_type defMID = 10;

    std::thread thr ([&] {
            std::this_thread::sleep_for (std::chrono::milliseconds (50));
            mq[idx]->enqueue<mqmx::message> (defMID);
        });

    auto mqlist = mqmx::poll (std::begin (mq), std::end (mq),
			      mqmx::wait_time_provider::WAIT_INFINITELY);
    if (thr.joinable ())
    {
        thr.join ();
    }

    ASSERT_EQ (1, mqlist.size ());
    ASSERT_EQ (mq[idx]->get_qid (), mqlist.front ().get_qid ());
    ASSERT_EQ (mqmx::message_queue::notification_flag::data, mqlist.front ().get_flags ());
}

TEST_F (MQPollFixture, absolute_timeout)
{
    const size_t NREPS = 3;
    for (size_t ix = 0; ix < NREPS; ++ix)
    {
        auto mqlist = mqmx::poll (std::begin (mq), std::end (mq),
				  std::chrono::steady_clock::now () + std::chrono::microseconds (1));
        ASSERT_EQ (0, mqlist.size ());
    }
}

TEST_F (MQPollFixture, relative_timeout)
{
    const size_t NREPS = 3;
    for (size_t ix = 0; ix < NREPS; ++ix)
    {
        auto mqlist = mqmx::poll (std::begin (mq), std::end (mq), std::chrono::microseconds (1));
        ASSERT_EQ (0, mqlist.size ());
    }
}
