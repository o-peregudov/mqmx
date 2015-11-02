#include "mqmx/message_queue_poll.h"
#include <cassert>
#include <thread>
#include <iostream>

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
	using namespace mqmx;
	const size_t stride = 3;
	size_t nqueues_signaled = 0;
	for (size_t ix = 0; ix < NQUEUES; ix += stride)
	{
	    mq[ix]->push (message_queue::message_ptr_type (
			      new message (mq[ix]->get_id (), 0)));
	    ++nqueues_signaled;
	}
	auto mqlist = sut.poll (std::begin (mq), std::end (mq));
	assert ((mqlist.size () == nqueues_signaled) &&
		("Number of signaled queues should be correct"));
    }

    void signal_while_infinte_wait_test ()
    {
	using namespace mqmx;
	const size_t idx = NQUEUES - 1;
	std::thread thr ([&] {
		std::this_thread::sleep_for (std::chrono::milliseconds (50));
		mq[idx]->push (message_queue::message_ptr_type (
				   new message (mq[idx]->get_id (), 0)));
	    });
	auto mqlist = sut.poll (std::begin (mq), std::end (mq),
				wait_time_provider::WAIT_INFINITELY);
	if (thr.joinable ())
	{
	    thr.join ();
	}
	assert ((mqlist.size () == 1) &&
		("Number of signaled queues should be correct"));
	assert ((std::get<0> (mqlist.front ()) == mq[idx]->get_id ()) &&
		("Proper QID should be reported"));
    }

    void relative_timeout_test ()
    {
	using namespace mqmx;
	for (size_t ix = 0; ix < 3; ++ix)
	{
	    auto mqlist = sut.poll (std::begin (mq), std::end (mq),
				    std::chrono::milliseconds (10));
	    assert ((mqlist.size () == 0) &&
		    ("There should be no events reported!"));
	}
    }

    void absolute_timeout_test ()
    {
	using namespace mqmx;
	for (size_t ix = 0; ix < 3; ++ix)
	{
	    auto mqlist = sut.poll (std::begin (mq), std::end (mq),
				    std::chrono::steady_clock::now () +
				    std::chrono::milliseconds (10));
	    assert ((mqlist.size () == 0) &&
		    ("There should be no events reported!"));
	}
    }
};

int main (int argc, const char ** argv)
{
    {
	test_fixture fixture;
	fixture.sanity_test ();
    }
    {
	test_fixture fixture;
	fixture.initial_notification_test ();
    }
    {
	test_fixture fixture;
	fixture.signal_while_infinte_wait_test ();
    }
    {
	test_fixture fixture;
	fixture.relative_timeout_test ();
    }
    {
	test_fixture fixture;
	fixture.absolute_timeout_test ();
    }
    return 0;
}
