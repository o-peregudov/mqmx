#include <mqmx/message_queue_poll.h>

namespace mqmx
{
    message_queue_poll::message_queue_poll ()
        : _notifications_mutex ()
        , _notifications_condition ()
        , _notifications ()
    {
    }

    message_queue_poll::~message_queue_poll ()
    {
    }

    void message_queue_poll::notify (const queue_id_type qid,
                                     message_queue * mq,
                                     const message_queue::notification_flags_type flag)
    {
        try
        {
            const notification_rec_type elem (qid, mq, flag);
            const auto compare = [](const notification_rec_type & a,
                                    const notification_rec_type & b) {
                return ((a.get_qid () <= b.get_qid ()) &&
                        (a.get_mq () < b.get_mq ()));
            };

            lock_type notifications_guard (_notifications_mutex);
            notifications_list_type::iterator iter = std::upper_bound (
                _notifications.begin (), _notifications.end (), elem, compare);
            if (iter != _notifications.begin ())
            {
                notifications_list_type::iterator prev = iter;
                --prev;

                if ((prev->get_qid () == qid) && (prev->get_mq () == mq))
                {
                    prev->get_flags () |= flag;
                    _notifications_condition.notify_one ();
                    return; /* queue already has some notification(s) */
                }
            }
            _notifications.insert (iter, elem);
            _notifications_condition.notify_one ();
        }
        catch (...)
        { }
    }
} /* namespace mqmx */
