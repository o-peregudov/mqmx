#include "test/fixtures/MessageQueuePoll.h"

namespace fixtures
{
    MessageQueuePoll::MessageQueuePoll ()
	: mq ()
    {
	for (size_t ix = 0; ix < NQUEUES; ++ix)
	{
	    mq.emplace_back (new mqmx::MessageQueue (ix));
	}
    }
} /* namespace fixtures */