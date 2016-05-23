#include "mqmx/message_queue_pool.h"
#include <algorithm>

namespace mqmx
{
    const queue_id_type   message_queue_pool::CONTROL_MESSAGE_QUEUE_ID = 0x00;
    const message_id_type message_queue_pool::TERMINATE_MESSAGE_ID = 0x00;
    const message_id_type message_queue_pool::POLL_PAUSE_MESSAGE_ID = 0x01;
    const message_id_type message_queue_pool::ADD_QUEUE_MESSAGE_ID = 0x02;
    const message_id_type message_queue_pool::REMOVE_QUEUE_MESSAGE_ID = 0x03;

    struct message_queue_pool::add_queue_message : message
    {
        message_queue * mq;
        semaphore_type * sem;

        add_queue_message (const queue_id_type queue_id,
                           message_queue * q, semaphore_type * s)
            : message (queue_id, message_queue_pool::ADD_QUEUE_MESSAGE_ID)
            , mq (q)
            , sem (s)
        { }
    };

    struct message_queue_pool::remove_queue_message : message
    {
        const message_queue * mq;
        message_queue_pool::semaphore_type * sem;

        remove_queue_message (const queue_id_type queue_id,
                              const message_queue * q, semaphore_type * s)
            : message (queue_id, message_queue_pool::REMOVE_QUEUE_MESSAGE_ID)
            , mq (q)
            , sem (s)
        { }
    };

    status_code message_queue_pool::controlQueueHandler (message::upointer_type && msg)
    {
        if (msg->get_mid () == TERMINATE_MESSAGE_ID)
        {
            return ExitStatus::HaltRequested;
        }

        if (msg->get_mid () == POLL_PAUSE_MESSAGE_ID)
        {
            return ExitStatus::PauseRequested;
        }

        if (msg->get_mid () == ADD_QUEUE_MESSAGE_ID)
        {
            add_queue_message * aqmsg = static_cast<add_queue_message *> (msg.get ());
            auto it = std::begin (m_mqs);
            while ((++it != std::end (m_mqs)) && ((*it)->get_qid () < aqmsg->mq->get_qid ()));
            assert ((it == std::end (m_mqs)) || (aqmsg->mq->get_qid () < (*it)->get_qid ()));
            m_mqs.insert (it, aqmsg->mq);
            aqmsg->sem->post ();
            return ExitStatus::Success;
        }

        if (msg->get_mid () == REMOVE_QUEUE_MESSAGE_ID)
        {
            remove_queue_message * rqmsg = static_cast<remove_queue_message *> (msg.get ());
            auto it = std::find (std::begin (m_mqs), std::end (m_mqs), rqmsg->mq);
            if (it != std::end (m_mqs))
            {
                m_mqs.erase (it);
            }
            rqmsg->sem->post ();
            return ExitStatus::RestartNeeded;
        }

        return ExitStatus::Success;
    }

    status_code message_queue_pool::handleNotifications (
        const message_queue_poll::notification_rec_type & rec)
    {
        if (rec.get_flags () & (message_queue::notification_flag::closed|
				message_queue::notification_flag::detached))
        {
            /* pointer to message queue is no longer valid */
        }
        else if (rec.get_flags () & message_queue::notification_flag::data)
        {
            assert (rec.get_mq () != nullptr);
            assert (rec.get_qid () < m_mqHandler.size ());

            message::upointer_type msg = rec.get_mq ()->pop ();
            const status_code retCode = (m_mqHandler[rec.get_qid ()])(std::move (msg));
            if (retCode != ExitStatus::Success)
            {
                /* TODO: print diagnostic message here */
            }
            return retCode;
        }
        return ExitStatus::Success;
    }

    void message_queue_pool::threadLoop ()
    {
        for (;;)
        {
            message_queue_poll mqp;
            const auto mqlist = mqp.poll (std::begin (m_mqs), std::end (m_mqs),
                                          wait_time_provider::WAIT_INFINITELY);
            size_t starti = 0;
            if (mqlist.front ().get_qid () == m_mqControl.get_qid ())
            {
                const status_code retCode = handleNotifications (mqlist.front ());
                if (retCode == ExitStatus::HaltRequested)
                {
                    break;
                }

                if (retCode == ExitStatus::RestartNeeded)
                {
                    continue;
                }

                if (retCode == ExitStatus::PauseRequested)
                {
                    m_pauseSemaphore.post ();
                    m_resumeSemaphore.wait ();
                    continue;
                }

                ++starti;
            }

            for (size_t i = starti; i < mqlist.size (); ++i)
            {
                try
                {
                    handleNotifications (mqlist[i]);
                }
                catch (...)
                {
                    /* TODO: consider to add '#pragma omp cancel for' */
                }
            }
        }
    }

    bool message_queue_pool::isPollIdle ()
    {
        m_mqControl.enqueue<message> (POLL_PAUSE_MESSAGE_ID);
        m_pauseSemaphore.wait ();

        message_queue_poll mqp;
        const bool idleStatus = mqp.poll (std::begin (m_mqs), std::end (m_mqs)).empty ();

        m_resumeSemaphore.post ();
        return idleStatus;
    }

    message_queue_pool::message_queue_pool (const size_t capacity)
        : m_mqControl (CONTROL_MESSAGE_QUEUE_ID)
        , m_mqHandler ()
        , m_mqs ()
        , m_pauseSemaphore ()
        , m_resumeSemaphore ()
        , m_auxThread ()
    {
        m_mqHandler.resize (capacity + 1);
        m_mqHandler[m_mqControl.get_qid ()] = std::bind (
            &message_queue_pool::controlQueueHandler, this, std::placeholders::_1);

        m_mqs.reserve (capacity + 1);
        m_mqs.emplace_back (&m_mqControl);

        std::thread auxiliary_thread ([this]{ threadLoop (); });
        m_auxThread.swap (auxiliary_thread);
    }

    message_queue_pool::~message_queue_pool ()
    {
        m_mqControl.enqueue<message> (TERMINATE_MESSAGE_ID);
        m_auxThread.join ();
    }

    message_queue_pool::mq_upointer_type message_queue_pool::allocateQueue (
        const message_handler_func_type & handler)
    {
        if (!handler)
        {
            return mq_upointer_type ();
        }

        auto it = std::begin (m_mqHandler);
        while ((++it != std::end (m_mqHandler)) && *it);
        const queue_id_type qid = std::distance (std::begin (m_mqHandler), it);
        assert (qid < m_mqHandler.size ());

        mq_upointer_type mq (new message_queue (qid), mq_deleter (this));
        m_mqHandler[qid] = handler;

        semaphore_type sem;
        if (m_mqControl.enqueue<add_queue_message> (mq.get (), &sem) == ExitStatus::Success)
        {
            sem.wait ();
            return mq;
        }
        return mq_upointer_type ();
    }

    status_code message_queue_pool::removeQueue (const message_queue * const mq)
    {
        if (mq == nullptr)
        {
            return ExitStatus::InvalidArgument;
        }

        if (!(mq->get_qid () < m_mqHandler.size ()) || !m_mqHandler[mq->get_qid ()])
        {
            return ExitStatus::NotFound;
        }

        semaphore_type sem;
        m_mqControl.enqueue<remove_queue_message> (mq, &sem);
        sem.wait ();

        return ExitStatus::Success;
    }
} /* namespace mqmx */
