#include "test/fixtures/poll.h"

#include <cassert>

struct test_fixture : fixtures::poll
{
    mqmx::MessageQueuePoll sut;

    void initial_notification_test ()
    {
	using namespace mqmx;
	const size_t STRIDE = 3;
	size_t nqueues_signaled = 0;
	for (size_t ix = 0; ix < NQUEUES; ix += STRIDE)
	{
	    mq[ix]->push (Message::upointer_type (new Message (mq[ix]->getQID (), 0)));
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
    fixture.initial_notification_test ();
    return 0;
}
