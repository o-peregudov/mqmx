#include <mqmx/MessageQueue.h>
#include <cassert>

namespace mqmx
{
    MessageQueue::MessageQueue (const queue_id_type ID)
        : m_id (ID)
        , m_mutex ()
        , m_queue ()
        , m_listener (nullptr)
    {
    }

    MessageQueue::MessageQueue (MessageQueue && o)
        : m_id (Message::UndefinedQID)
        , m_mutex ()
        , m_queue ()
        , m_listener (nullptr)
    {
        lock_type guard (o.m_mutex);
        std::swap (m_queue, o.m_queue);
        std::swap (m_id, o.m_id);
        std::swap (m_listener, o.m_listener);
        if (m_listener)
        {
            auto cplistener (m_listener);
            m_listener = nullptr;

            cplistener->notify (m_id, &o, NotificationFlag::Detached);
        }
    }

    MessageQueue & MessageQueue::operator = (MessageQueue && o)
    {
        if (this != &o)
        {
            std::lock (m_mutex, o.m_mutex);
            lock_type guard_this (m_mutex, std::adopt_lock_t ());
            lock_type guard_o (o.m_mutex, std::adopt_lock_t ());
            if (m_listener)
            {
                m_listener->notify (m_id, this, NotificationFlag::Detached);
                m_listener = nullptr;
            }
            m_queue.clear ();
            m_id = Message::UndefinedQID;
            std::swap (m_queue, o.m_queue);
            std::swap (m_id, o.m_id);
            std::swap (m_listener, o.m_listener);
            if (m_listener)
            {
                auto cplistener (m_listener);
                m_listener = nullptr;

                cplistener->notify (m_id, &o, NotificationFlag::Detached);
            }
        }
        return *this;
    }

    MessageQueue::~MessageQueue ()
    {
        if (m_listener)
        {
            m_listener->notify (m_id, nullptr, NotificationFlag::Closed);
        }
    }

    queue_id_type MessageQueue::getQID () const
    {
        return m_id;
    }

    status_code MessageQueue::push (Message::upointer_type && msg)
    {
        if (msg.get () == nullptr)
        {
            return ExitStatus::InvalidArgument;
        }

        lock_type guard (m_mutex);
        if ((m_id == Message::UndefinedQID) ||
            (m_id != msg->getQID ()))
        {
            return ExitStatus::NotSupported;
        }

        m_queue.push_back (std::move (msg));
        if (m_listener && (m_queue.size () == 1))
        {
            /* only first message will be reported */
            m_listener->notify (m_id, this, NotificationFlag::NewData);
        }
	return ExitStatus::Success;
    }

    Message::upointer_type MessageQueue::pop ()
    {
        Message::upointer_type msg;
        lock_type guard (m_mutex);
        if ((m_id != Message::UndefinedQID) && !m_queue.empty ())
        {
            msg = std::move (m_queue.front ());
            m_queue.pop_front ();
        }
        return std::move (msg);
    }

    status_code MessageQueue::setListener (Listener & l)
    {
        lock_type guard (m_mutex);
        if (m_listener)
        {
            return ExitStatus::AlreadyExist;
        }

        m_listener = &l;
        if (!m_queue.empty ())
        {
            m_listener->notify (m_id, this, NotificationFlag::NewData);
        }
        return ExitStatus::Success;
    }

    void MessageQueue::clearListener ()
    {
        lock_type guard (m_mutex);
        m_listener = nullptr;
    }
} /* namespace mqmx */
