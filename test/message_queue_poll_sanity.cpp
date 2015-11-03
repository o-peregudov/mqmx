#include "test/fixture/poll.h"

#include <cassert>

struct test_fixture : fixture::poll
{
    mqmx::MessageQueuePoll sut;

    void sanity_test ()
    {
	auto mqlist = sut.poll (std::begin (mq), std::end (mq));
	assert ((mqlist.empty () == true) &&
		("No events should be reported"));
    }
};

int main (int argc, const char ** argv)
{
    test_fixture fixture;
    fixture.sanity_test ();
    return 0;
}
