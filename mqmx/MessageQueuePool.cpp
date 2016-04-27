#include "mqmx/MessageQueuePool.h"

namespace mqmx
{
    status_code MessageQueuePool::controlQueueHandler (Message::upointer_type && msg)
    {
        if (msg->getMID () == TERMINATE_MESSAGE_ID)
        {
            m_terminateFlag = true;
        }
        else if (msg->getMID () == POLL_PAUSE_MESSAGE_ID)
        {
            lock_type guard (m_pollMutex);
            m_pauseFlag = true;
        }
        else
        {
            return ExitStatus::Success;
        }
        return ExitStatus::RestartNeeded;
    }

    status_code MessageQueuePool::handleNotifications (
        const MessageQueuePoll::notification_rec_type & rec)
    {
        if (rec.getFlags () & MessageQueue::NotificationFlag::NewData)
        {
            Message::upointer_type msg = rec.getMQ ()->pop ();
            const auto it = m_mqHandler.find (rec.getQID ());
            if (it->second)
            {
                const status_code retCode = (it->second)(std::move (msg));
                if (retCode != ExitStatus::Success)
                {
                    /* TODO: print diagnostic message here */
                }
                return retCode;
            }
        }
        return ExitStatus::Success;
    }

    void MessageQueuePool::threadLoop ()
    {
        m_mqs.push_back (&m_mqControl);
        for (;;)
        {
            MessageQueuePoll mqp;
            const auto mqlist = mqp.poll (std::begin (m_mqs), std::end (m_mqs),
                                          WaitTimeProvider::WAIT_INFINITELY);
#pragma omp parallel
            {
#pragma omp for
                for (size_t i = 0; i < mqlist.size (); ++i)
                {
                    const status_code retCode = handleNotifications (mqlist[i]);
                    if (retCode == ExitStatus::RestartNeeded)
                    {
#pragma omp cancel for
                    }
                }
            }

            if (m_terminateFlag)
            {
                break;
            }

            lock_type guard (m_pollMutex);
            if (m_pauseFlag)
            {
                m_pollCondition.notify_one ();
                m_pollCondition.wait (guard, [this]{ return !m_pauseFlag; });
            }
        }
    }

    bool MessageQueuePool::isPollIdle ()
    {
        lock_type guard (m_pollMutex);
        m_mqControl.enqueue<Message> (POLL_PAUSE_MESSAGE_ID);
        m_pollCondition.wait (guard, [this]{ return m_pauseFlag; });

        MessageQueuePoll mqp;
        const bool idleStatus = mqp.poll (std::begin (m_mqs), std::end (m_mqs)).empty ();

        m_pauseFlag = false;
        m_pollCondition.notify_one ();
        return idleStatus;
    }

    MessageQueuePool::MessageQueuePool ()
        : m_mqControl (CONTROL_MESSAGE_QUEUE_ID)
        , m_mqHandler ()
        , m_mqs ()
        , m_terminateFlag (false)
        , m_pauseFlag (false)
        , m_pollMutex ()
        , m_pollCondition ()
        , m_auxThread ([this]{ threadLoop (); })
    {
        const message_handler_func_type handler = std::bind (
            &MessageQueuePool::controlQueueHandler, this, std::placeholders::_1);
        m_mqHandler.insert (std::make_pair (m_mqControl.getQID (), handler));
    }

    MessageQueuePool::~MessageQueuePool ()
    {
        {
            lock_type guard (m_pollMutex);
            if (m_pauseFlag)
            {
                m_pauseFlag = false;
                m_pollCondition.notify_one ();
            }
        }
        m_mqControl.enqueue<Message> (TERMINATE_MESSAGE_ID);
        m_auxThread.join ();
    }

    MessageQueue::upointer_type MessageQueuePool::addQueue (const message_handler_func_type & handler)
    {
        MessageQueue::upointer_type newMQ;
        if (handler)
        {
            lock_type guard (m_pollMutex);
            m_mqControl.enqueue<Message> (POLL_PAUSE_MESSAGE_ID);
            m_pollCondition.wait (guard, [this]{ return m_pauseFlag; });

            auto it = --(m_mqHandler.end ());
            newMQ.reset (new MessageQueue (it->first + 1));

            auto ires = m_mqHandler.insert (std::make_pair (newMQ->getQID (), handler));
            if (ires.second)
            {
                m_mqs.push_back (newMQ.get ());
            }

            m_pauseFlag = false;
            m_pollCondition.notify_one ();
        }
        return newMQ;
    }

    status_code MessageQueuePool::removeQueue (const MessageQueue * const mq)
    {
        if (mq == nullptr)
        {
            return ExitStatus::InvalidArgument;
        }

        lock_type guard (m_pollMutex);
        m_mqControl.enqueue<Message> (POLL_PAUSE_MESSAGE_ID);
        m_pollCondition.wait (guard, [this]{ return m_pauseFlag; });

        bool found = false;
        for (size_t ix = 0; ix < m_mqs.size (); ++ix)
        {
            if (m_mqs[ix] == mq)
            {
                found = true;
                std::swap (m_mqs[ix], m_mqs.back ());
                m_mqs.pop_back ();
                break;
            }
        }

        if (found)
        {
            m_mqHandler.erase (mq->getQID ());
        }

        m_pauseFlag = false;
        m_pollCondition.notify_one ();

        return (found ? ExitStatus::Success : ExitStatus::NotFound);
    }
} /* namespace mqmx */
