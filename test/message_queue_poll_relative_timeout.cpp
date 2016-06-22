#include "test/fixtures/message_queue_poll.h"

#undef NDEBUG
#include <cassert>

int main ()
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
