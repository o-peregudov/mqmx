#include "mqmx/message_queue_poll.h"
#include "test/fixture/poll.h"

#include <cassert>
#include <thread>

struct test_fixture : fixture::poll
{
    mqmx::message_queue_poll sut;

    void infinte_wait_test ()
    {
	using namespace mqmx;
	const size_t idx = NQUEUES - 1;
	std::thread thr ([&] {
		std::this_thread::sleep_for (std::chrono::milliseconds (50));
		mq[idx]->push (message_queue::message_ptr_type (
				   new Message (mq[idx]->get_id (), 0)));
	    });
	auto mqlist = sut.poll (std::begin (mq), std::end (mq),
				WaitTimeProvider::WAIT_INFINITELY);
	if (thr.joinable ())
	{
	    thr.join ();
	}
	assert ((mqlist.size () == 1) &&
		("Number of signaled queues should be correct"));
	assert ((std::get<0> (mqlist.front ()) == mq[idx]->get_id ()) &&
		("Proper QID should be reported"));
    }
};

int main (int argc, const char ** argv)
{
    test_fixture fixture;
    fixture.infinte_wait_test ();
    return 0;
}
