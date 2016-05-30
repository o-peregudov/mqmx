#include "mqmx/message_queue_poll.h"

#include <thread>

#undef NDEBUG
#include <cassert>

namespace fixtures
{
    struct message_queue_poll
    {
        static const size_t NQUEUES = 10;
        std::vector<mqmx::message_queue::upointer_type> mq;

        message_queue_poll ()
            : mq ()
        {
            for (size_t ix = 0; ix < NQUEUES; ++ix)
            {
                mq.emplace_back (new mqmx::message_queue (ix));
            }
        }
    };
} /* namespace fixtures */

int main (int argc, const char ** argv)
{
    fixtures::message_queue_poll fixture;

    const size_t idx = fixtures::message_queue_poll::NQUEUES - 1;
    const mqmx::message_id_type defMID = 10;

    std::thread thr ([&] {
	    std::this_thread::sleep_for (std::chrono::milliseconds (50));
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
