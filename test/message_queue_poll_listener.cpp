#include "mqmx/message_queue_poll.h"

#undef NDEBUG
#include <cassert>

int main (int argc, const char ** argv)
{
    /*
     * notifications ordering
     */
    const mqmx::queue_id_type aqid = 10;
    const mqmx::queue_id_type bqid = 20;
    const mqmx::message_id_type defmid = 10;

    mqmx::message_queue_poll_listener listener;
    {
        mqmx::message_queue aqueue (aqid);
        aqueue.enqueue<mqmx::message> (defmid);
        {
            mqmx::message_queue bqueue (bqid);
            bqueue.enqueue<mqmx::message> (defmid);
            bqueue.set_listener (listener);
        }
        aqueue.set_listener (listener);
    }
    auto mqlist = listener.get_notifications ();
    assert (2 == mqlist.size ());
    assert (aqid == mqlist.front ().get_qid ());
    assert (bqid == mqlist.back ().get_qid ());
    return 0;
}
