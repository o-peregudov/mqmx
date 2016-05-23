#include <mqmx/message_queue_poll.h>

namespace mqmx
{
    message_queue_poll::message_queue_poll ()
        : m_notifications_mutex ()
        , m_notifications_condition ()
        , m_notifications ()
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

            lock_type notifications_guard (m_notifications_mutex);
            notifications_list_type::iterator iter = std::upper_bound (
                m_notifications.begin (), m_notifications.end (), elem, compare);
            if (iter != m_notifications.begin ())
            {
                notifications_list_type::iterator prev = iter;
                --prev;

                if ((prev->get_qid () == qid) && (prev->get_mq () == mq))
                {
                    prev->get_flags () |= flag;
                    m_notifications_condition.notify_one ();
                    return; /* queue already has some notification(s) */
                }
            }
            m_notifications.insert (iter, elem);
            m_notifications_condition.notify_one ();
        }
        catch (...)
        { }
    }
} /* namespace mqmx */
