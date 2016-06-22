#include "test/fixtures/message_queue_poll.h"

#undef NDEBUG
#include <cassert>

int main ()
{
    fixtures::message_queue_poll fixture;
    auto mqlist = mqmx::poll (std::begin (fixture.mq), std::end (fixture.mq));
    assert (mqlist.empty ());
    return 0;
}
