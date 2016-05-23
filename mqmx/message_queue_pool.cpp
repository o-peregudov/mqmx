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

    status_code message_queue_pool::control_queue_handler (message::upointer_type && msg)
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
            auto it = std::begin (_mqs);
            while ((++it != std::end (_mqs)) && ((*it)->get_qid () < aqmsg->mq->get_qid ()));
            assert ((it == std::end (_mqs)) || (aqmsg->mq->get_qid () < (*it)->get_qid ()));
            _mqs.insert (it, aqmsg->mq);
            aqmsg->sem->post ();
            return ExitStatus::Success;
        }

        if (msg->get_mid () == REMOVE_QUEUE_MESSAGE_ID)
        {
            remove_queue_message * rqmsg = static_cast<remove_queue_message *> (msg.get ());
            auto it = std::find (std::begin (_mqs), std::end (_mqs), rqmsg->mq);
            if (it != std::end (_mqs))
            {
                _mqs.erase (it);
            }
            rqmsg->sem->post ();
            return ExitStatus::RestartNeeded;
        }

        return ExitStatus::Success;
    }

    status_code message_queue_pool::handle_notifications (
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
            assert (rec.get_qid () < _mqHandler.size ());

            message::upointer_type msg = rec.get_mq ()->pop ();
            const status_code retCode = (_mqHandler[rec.get_qid ()])(std::move (msg));
            if (retCode != ExitStatus::Success)
            {
                /* TODO: print diagnostic message here */
            }
            return retCode;
        }
        return ExitStatus::Success;
    }

    void message_queue_pool::thread_loop ()
    {
        for (;;)
        {
            message_queue_poll mqp;
            const auto mqlist = mqp.poll (std::begin (_mqs), std::end (_mqs),
                                          wait_time_provider::WAIT_INFINITELY);
            size_t starti = 0;
            if (mqlist.front ().get_qid () == _mqControl.get_qid ())
            {
                const status_code retCode = handle_notifications (mqlist.front ());
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
                    _pauseSemaphore.post ();
                    _resumeSemaphore.wait ();
                    continue;
                }

                ++starti;
            }

            for (size_t i = starti; i < mqlist.size (); ++i)
            {
                try
                {
                    handle_notifications (mqlist[i]);
                }
                catch (...)
                {
                    /* TODO: consider to add '#pragma omp cancel for' */
                }
            }
        }
    }

    bool message_queue_pool::is_poll_idle ()
    {
        _mqControl.enqueue<message> (POLL_PAUSE_MESSAGE_ID);
        _pauseSemaphore.wait ();

        message_queue_poll mqp;
        const bool idleStatus = mqp.poll (std::begin (_mqs), std::end (_mqs)).empty ();

        _resumeSemaphore.post ();
        return idleStatus;
    }

    message_queue_pool::message_queue_pool (const size_t capacity)
        : _mqControl (CONTROL_MESSAGE_QUEUE_ID)
        , _mqHandler ()
        , _mqs ()
        , _pauseSemaphore ()
        , _resumeSemaphore ()
        , _auxThread ()
    {
        _mqHandler.resize (capacity + 1);
        _mqHandler[_mqControl.get_qid ()] = std::bind (
            &message_queue_pool::control_queue_handler, this, std::placeholders::_1);

        _mqs.reserve (capacity + 1);
        _mqs.emplace_back (&_mqControl);

        thread_type auxiliary_thread ([this]{ thread_loop (); });
        _auxThread.swap (auxiliary_thread);
    }

    message_queue_pool::~message_queue_pool ()
    {
        _mqControl.enqueue<message> (TERMINATE_MESSAGE_ID);
        _auxThread.join ();
    }

    message_queue_pool::mq_upointer_type message_queue_pool::allocate_queue (
        const message_handler_func_type & handler)
    {
        if (!handler)
        {
            return mq_upointer_type ();
        }

        auto it = std::begin (_mqHandler);
        while ((++it != std::end (_mqHandler)) && *it);
        const queue_id_type qid = std::distance (std::begin (_mqHandler), it);
        assert (qid < _mqHandler.size ());

        mq_upointer_type mq (new message_queue (qid), mq_deleter (this));
        _mqHandler[qid] = handler;

        semaphore_type sem;
        if (_mqControl.enqueue<add_queue_message> (mq.get (), &sem) == ExitStatus::Success)
        {
            sem.wait ();
            return mq;
        }
        return mq_upointer_type ();
    }

    status_code message_queue_pool::remove_queue (const message_queue * const mq)
    {
        if (mq == nullptr)
        {
            return ExitStatus::InvalidArgument;
        }

        if (!(mq->get_qid () < _mqHandler.size ()) || !_mqHandler[mq->get_qid ()])
        {
            return ExitStatus::NotFound;
        }

        semaphore_type sem;
        _mqControl.enqueue<remove_queue_message> (mq, &sem);
        sem.wait ();

        return ExitStatus::Success;
    }
} /* namespace mqmx */
