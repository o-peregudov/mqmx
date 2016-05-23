#pragma once

#include <mqmx/MessageQueuePoll.h>

namespace fixtures
{
    struct MessageQueuePollFixture
    {
	static const size_t NQUEUES = 10;
	std::vector<mqmx::message_queue::upointer_type> mq;

	MessageQueuePollFixture ();
    };
} /* namespace fixtures */
