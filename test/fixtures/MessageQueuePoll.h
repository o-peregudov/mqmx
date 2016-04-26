#pragma once

#include <mqmx/MessageQueuePoll.h>

namespace fixtures
{
    struct MessageQueuePoll
    {
	static const size_t NQUEUES = 10;
	std::vector<mqmx::MessageQueue::upointer_type> mq;
	MessageQueuePoll ();
    };
} /* namespace fixtures */
