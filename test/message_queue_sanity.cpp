#include "mqmx/message_queue.h"

#undef NDEBUG
#include <cassert>

int main ()
{
    {
        /*
         * sanity checks
         */
        using namespace mqmx;
        const queue_id_type defQID = 10;
        const message_id_type defMID = 10;

        message_queue queue (defQID);
        message::upointer_type msg (queue.pop ());
        assert (nullptr == msg.get ());

        status_code retCode = queue.push (nullptr);
        assert (ExitStatus::InvalidArgument == retCode);

        {
            message_queue queue2 (defQID + 1);
            retCode = queue.push (queue2.new_message<message> (defQID));
            assert (ExitStatus::NotSupported == retCode);
        }

        retCode = queue.enqueue<message> (defMID);
        assert (ExitStatus::Success == retCode);
    }
    {
        /*
         * FIFO order
         */
        using namespace mqmx;
        const queue_id_type defQID = 10;
        const message_id_type defMID = 10;

        message_queue queue (defQID);
        for (size_t ix = 0; ix < 10; ++ix)
        {
            status_code retCode = queue.enqueue<message> (defMID + ix);
            assert (ExitStatus::Success == retCode);
        }

        message::upointer_type msg;
        for (size_t ix = 0; ix < 10; ++ix)
        {
            msg = queue.pop ();
            assert (nullptr != msg.get ());
            assert (defQID == msg->get_qid ());
            assert ((defMID + ix) == msg->get_mid ());
        }

        msg = queue.pop ();
        assert (nullptr == msg.get ());
    }
    return 0;
}
