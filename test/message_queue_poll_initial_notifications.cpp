#include "test/fixtures/message_queue_poll.h"

#undef NDEBUG
#include <cassert>

int main ()
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
