#pragma once
#include <mqmx/MessageQueuePoll.h>

namespace fixtures
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
} /* namespace fixtures */

