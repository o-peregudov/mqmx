#include "mqmx/message_queue_poll.h"

#include <thread>
#include <cstdlib>
#include <ctime>

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
    {
        /*
         * sanity checks
         */
        fixtures::message_queue_poll fixture;

        auto mqlist = mqmx::poll (std::begin (fixture.mq), std::end (fixture.mq));
        assert (mqlist.empty ());
    }
    {
        /*
         * initial notifications
         */
        fixtures::message_queue_poll fixture;

        srand (time (nullptr));
        const size_t STRIDE
            = static_cast<double> (rand ()) / RAND_MAX
            * fixtures::message_queue_poll::NQUEUES;

        size_t nqueues_signaled = 0;
        for (size_t ix = 0; ix < fixtures::message_queue_poll::NQUEUES; ix += STRIDE)
        {
            fixture.mq[ix]->enqueue<mqmx::message> (0);
            ++nqueues_signaled;
        }

        auto mqlist = mqmx::poll (std::begin (fixture.mq), std::end (fixture.mq));
        assert (nqueues_signaled == mqlist.size ());
    }
    {
        /*
         * inifinite wait
         */
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
    }
    {
        /*
         * absolute timeout
         */
        fixtures::message_queue_poll fixture;

        const size_t NREPS = 3;
        for (size_t ix = 0; ix < NREPS; ++ix)
        {
            auto mqlist = mqmx::poll (std::begin (fixture.mq), std::end (fixture.mq),
                                      std::chrono::steady_clock::now () + std::chrono::microseconds (1));
            assert (0 == mqlist.size ());
        }
    }
    {
        /*
         * relative timeout
         */
        fixtures::message_queue_poll fixture;

        const size_t NREPS = 3;
        for (size_t ix = 0; ix < NREPS; ++ix)
        {
            auto mqlist = mqmx::poll (std::begin (fixture.mq), std::end (fixture.mq),
                                      std::chrono::microseconds (1));
            assert (0 == mqlist.size ());
        }
    }
    return 0;
}
