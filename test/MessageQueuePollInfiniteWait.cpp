#if defined (NDEBUG)
#  undef NDEBUG
#endif
#include "test/fixtures/MessageQueuePoll.h"

#include <cassert>
#include <thread>

struct test_fixture : fixtures::MessageQueuePoll
{
    mqmx::MessageQueuePoll sut;

    void infinte_wait_test ()
    {
        using namespace mqmx;
        const size_t idx = NQUEUES - 1;
        std::thread thr ([&] {
                std::this_thread::sleep_for (std::chrono::milliseconds (50));
                mq[idx]->push (mq[idx]->newMessage<Message> (0));
            });
        auto mqlist = sut.poll (std::begin (mq), std::end (mq),
                                WaitTimeProvider::WAIT_INFINITELY);
        if (thr.joinable ())
        {
            thr.join ();
        }
        assert ((mqlist.size () == 1) &&
                ("Number of signaled queues should be correct"));
        assert ((mqlist.front ().getQID () == mq[idx]->getQID ()) &&
                ("Proper QID should be reported"));
    }
};

int main (int argc, const char ** argv)
{
    test_fixture fixture;
    fixture.infinte_wait_test ();
    return 0;
}
