#include <mqmx/MessageQueuePoll.h>

namespace mqmx
{
    MessageQueuePoll::MessageQueuePoll ()
        : m_poll_mutex ()
        , m_notifications_mutex ()
        , m_notifications_condition ()
        , m_notifications ()
    {
    }

    MessageQueuePoll::~MessageQueuePoll ()
    {
    }

    void MessageQueuePoll::notify (const queue_id_type qid,
                                   MessageQueue * mq,
                                   const MessageQueue::notification_flags_type flag)
    {
        try
        {
            const notification_rec_type elem (qid, mq, flag);
            const auto compare = [](const notification_rec_type & a,
                                    const notification_rec_type & b) {
                return ((a.getQID () <= b.getQID ()) &&
                        (a.getMQ () < b.getMQ ()));
            };

            lock_type notifications_guard (m_notifications_mutex);
            notifications_list_type::iterator iter = std::upper_bound (
                m_notifications.begin (), m_notifications.end (), elem, compare);
            if (iter != m_notifications.begin ())
            {
                notifications_list_type::iterator prev = iter;
                --prev;

                if ((prev->getQID () == qid) && (prev->getMQ () == mq))
                {
                    prev->getFlags () |= flag;
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
