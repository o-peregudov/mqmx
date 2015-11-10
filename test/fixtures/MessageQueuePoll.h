#pragma once

#include <mqmx/MessageQueuePoll.h>

namespace fixtures
{
    struct MessageQueuePoll
    {
	static const size_t NQUEUES = 10;
	std::vector<mqmx::MessageQueue::upointer_type> mq;

	MessageQueuePoll ()
	    : mq ()
	{
	    for (size_t ix = 0; ix < NQUEUES; ++ix)
	    {
		mq.emplace_back (new mqmx::MessageQueue (ix));
	    }
	}
    };
} /* namespace fixtures */
