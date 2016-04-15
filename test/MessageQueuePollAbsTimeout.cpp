#if defined (NDEBUG)
#  undef NDEBUG
#endif
#include "test/fixtures/MessageQueuePoll.h"

#include <cassert>

struct test_fixture : fixtures::MessageQueuePoll
{
    mqmx::MessageQueuePoll sut;

    void absolute_timeout_test ()
    {
	using namespace mqmx;
	const size_t NREPS = 3;
	for (size_t ix = 0; ix < NREPS; ++ix)
	{
	    auto mqlist = sut.poll (std::begin (mq), std::end (mq),
				    std::chrono::steady_clock::now () +
				    std::chrono::microseconds (1));
	    assert ((mqlist.size () == 0) &&
		    ("There should be no events reported!"));
	}
    }
};

int main (int argc, const char ** argv)
{
    test_fixture fixture;
    fixture.absolute_timeout_test ();
    return 0;
}
