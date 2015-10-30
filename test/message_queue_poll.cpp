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

    void sanity_test ()
    {
	auto mqlist = sut.poll (std::begin (mq), std::end (mq));
	assert ((mqlist.empty () == true) &&
		("No events should be reported"));
    }

    void initial_notification_test ()
    {
	const size_t stride = 3;
	size_t nqueues_signaled = 0;
	for (size_t ix = 0; ix < NQUEUES; ix += stride)
	{
	    mq[ix]->push (mqmx::message_queue::message_ptr_type (
			      new mqmx::message (mq[ix]->get_id (), 0)));
	    ++nqueues_signaled;
	}
	auto mqlist = sut.poll (std::begin (mq), std::end (mq));
	assert ((mqlist.size () == nqueues_signaled) &&
		("Number of signaled queues should be correct"));
    }
};

int main (int argc, const char ** argv)
{
    test_fixture fixture;
    fixture.sanity_test ();
    return 0;
}
