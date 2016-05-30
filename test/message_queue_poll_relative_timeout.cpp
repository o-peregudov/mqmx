#include "mqmx/message_queue_poll.h"

#undef NDEBUG
#include <cassert>

namespace fixtures
{
    struct message_queue_poll
    {
        static const size_t NQUEUES = 10;
        std::vector<mqmx::message_queue::upointer_type> mq;

        message_queue_poll ()
            : mq ()
        {
            for (size_t ix = 0; ix < NQUEUES; ++ix)
            {
                mq.emplace_back (new mqmx::message_queue (ix));
            }
        }
    };
} /* namespace fixtures */

int main (int argc, const char ** argv)
{
    fixtures::message_queue_poll fixture;

    const size_t NREPS = 3;
    for (size_t ix = 0; ix < NREPS; ++ix)
    {
	auto mqlist = mqmx::poll (std::begin (fixture.mq), std::end (fixture.mq),
				  std::chrono::microseconds (1));
	assert (0 == mqlist.size ());
    }
    return 0;
}
