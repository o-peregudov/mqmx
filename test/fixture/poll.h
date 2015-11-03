#ifndef MQMX_TEST_FIXTURE_POLL_H_INCLUDE
#define MQMX_TEST_FIXTURE_POLL_H_INCLUDE 1

#include "mqmx/message_queue_poll.h"

namespace fixture
{
    struct poll
    {
	static const size_t NQUEUES = 10;
	mqmx::MessageQueue * mq [NQUEUES];

	poll ()
	    : mq ()
	{
	    for (size_t ix = 0; ix < NQUEUES; ++ix)
	    {
		mq[ix] = new mqmx::MessageQueue (ix);
	    }
	}

	~poll ()
	{
	    for (size_t ix = 0; ix < NQUEUES; ++ix)
	    {
		delete mq[ix];
		mq[ix] = nullptr;
	    }
	}
    };
} /* namespace fixture */
#endif /* MQMX_TEST_FIXTURE_POLL_H_INCLUDE */
