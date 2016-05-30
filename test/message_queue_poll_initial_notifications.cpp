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

    const size_t STRIDE = 3;
    size_t nqueues_signaled = 0;
    for (size_t ix = 0; ix < fixtures::message_queue_poll::NQUEUES; ix += STRIDE)
    {
	fixture.mq[ix]->enqueue<mqmx::message> (0);
	++nqueues_signaled;
    }

    auto mqlist = mqmx::poll (std::begin (fixture.mq), std::end (fixture.mq));
    assert (nqueues_signaled == mqlist.size ());
    return 0;
}
