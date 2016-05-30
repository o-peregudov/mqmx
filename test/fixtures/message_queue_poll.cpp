#include "test/fixtures/message_queue_poll.h"

namespace fixtures
{
    message_queue_poll::message_queue_poll ()
	: mq ()
    {
	for (size_t ix = 0; ix < NQUEUES; ++ix)
	{
	    mq.emplace_back (new mqmx::message_queue (ix));
	}
    }
} /* namespace fixtures */
