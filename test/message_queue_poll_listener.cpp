#include "mqmx/message_queue_poll.h"
#include <gtest/gtest.h>

TEST (message_queue_poll_listener, ordering_of_notifications)
{
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
    ASSERT_EQ (2, mqlist.size ());
    ASSERT_EQ (aqid, mqlist.front ().get_qid ());
    ASSERT_EQ (bqid, mqlist.back ().get_qid ());
}
