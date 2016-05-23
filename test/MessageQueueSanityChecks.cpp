#include <mqmx/message_queue.h>
#include <gmock/gmock.h>

TEST (message_queue, sanity_checks)
{
    using namespace mqmx;
    const queue_id_type defQID = 10;
    const message_id_type defMID = 10;

    message_queue queue (defQID);
    message::upointer_type msg (queue.pop ());
    ASSERT_EQ (nullptr, msg.get ());

    status_code retCode = queue.push (nullptr);
    ASSERT_EQ (ExitStatus::InvalidArgument, retCode);

    {
	message_queue queue2 (defQID + 1);
	retCode = queue.push (queue2.new_message<message> (defQID));
	ASSERT_EQ (ExitStatus::NotSupported, retCode);
    }

    retCode = queue.enqueue<message> (defMID);
    ASSERT_EQ (ExitStatus::Success, retCode);
}

TEST (message_queue, fifo_ordering)
{
    using namespace mqmx;
    const queue_id_type defQID = 10;
    const message_id_type defMID = 10;

    message_queue queue (defQID);
    for (size_t ix = 0; ix < 10; ++ix)
    {
	status_code retCode = queue.enqueue<message> (defMID + ix);
	ASSERT_EQ (ExitStatus::Success, retCode);
    }

    message::upointer_type msg;
    for (size_t ix = 0; ix < 10; ++ix)
    {
        msg = queue.pop ();
	ASSERT_NE (nullptr, msg.get ());
	ASSERT_EQ (defQID, msg->get_qid ());
	ASSERT_EQ ((defMID + ix), msg->get_mid ());
    }

    msg = queue.pop ();
    ASSERT_EQ (nullptr, msg.get ());
}
