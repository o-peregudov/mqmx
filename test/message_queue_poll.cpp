#include "mqmx/message_queue_poll.h"
#include <cassert>

struct poll_fixture
{
    static const size_t NQUEUES = 10;
    mqmx::message_queue * mq [NQUEUES];

    poll_fixture ()
	: mq ()
    {
	for (size_t ix = 0; ix < NQUEUES; ++ix)
	{
	    mq[ix] = new mqmx::message_queue (ix);
	}
    }

    ~poll_fixture ()
    {
	for (size_t ix = 0; ix < NQUEUES; ++ix)
	{
	    delete mq[ix];
	    mq[ix] = nullptr;
	}
    }
};

struct test_fixture : poll_fixture
{
    mqmx::message_queue_poll sut;

    void sanity_check ()
    {
	auto mqlist = sut.poll (std::begin (mq), std::end (mq));
	assert ((mqlist.empty () == true) &&
		("No events should be reported"));
    }
};

int main (int argc, const char ** argv)
{
    test_fixture fixture;
    fixture.sanity_check ();
    return 0;
}
