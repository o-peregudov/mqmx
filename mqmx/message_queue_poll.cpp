#include <mqmx/message_queue_poll.h>

namespace mqmx
{
    message_queue_poll_listener::message_queue_poll_listener ()
        : _mutex ()
        , _condition ()
        , _notifications ()
    {
    }

    message_queue_poll_listener::~message_queue_poll_listener ()
    {
    }

    void message_queue_poll_listener::notify (const queue_id_type qid,
                                              message_queue * mq,
                                              const message_queue::notification_flags_type flag)
    {
        try
        {
            lock_type guard (_mutex);
            const notification_rec_type elem (qid, mq, flag);
            auto iter = std::upper_bound (
                _notifications.begin (), _notifications.end (), elem);
            if (iter != _notifications.begin ())
            {
                auto prev = iter;
                if ((--prev)->get_qid () == qid)
                {
                    prev->get_flags () |= flag;
                    _condition.notify_one ();
                    return; /* queue already has some notification(s) */
                }
            }
            _notifications.insert (iter, elem);
            _condition.notify_one ();
        }
        catch (...)
        { }
    }
} /* namespace mqmx */
