#include "test/fixtures/message_queue_poll.h"

#include <thread>

#undef NDEBUG
#include <cassert>

int main ()
{
    fixtures::message_queue_poll fixture;

    const size_t idx = fixtures::message_queue_poll::NQUEUES - 1;
    const mqmx::message_id_type defMID = 10;

    std::thread thr ([&] {
            std::this_thread::yield ();
            fixture.mq[idx]->enqueue<mqmx::message> (defMID);
        });

    auto mqlist = mqmx::poll (std::begin (fixture.mq), std::end (fixture.mq),
                              mqmx::wait_time_provider::WAIT_INFINITELY);
    if (thr.joinable ())
    {
        thr.join ();
    }

    assert (1 == mqlist.size ());
    assert (fixture.mq[idx]->get_qid () == mqlist.front ().get_qid ());
    assert (mqmx::message_queue::notification_flag::data == mqlist.front ().get_flags ());
    return 0;
}
