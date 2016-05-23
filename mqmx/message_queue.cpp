#include <mqmx/message_queue.h>
#include <cassert>

namespace mqmx
{
    message_queue::message_queue (const queue_id_type ID)
        : m_id (ID)
        , m_mutex ()
        , m_queue ()
        , m_listener (nullptr)
    {
    }

    message_queue::message_queue (message_queue && o)
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

            cplistener->notify (m_id, &o, notification_flag::Detached);
        }
    }

    message_queue & message_queue::operator = (message_queue && o)
    {
        if (this != &o)
        {
            std::lock (m_mutex, o.m_mutex);
            lock_type guard_this (m_mutex, std::adopt_lock_t ());
            lock_type guard_o (o.m_mutex, std::adopt_lock_t ());
            if (m_listener)
            {
                m_listener->notify (m_id, this, notification_flag::Detached);
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

                cplistener->notify (m_id, &o, notification_flag::Detached);
            }
        }
        return *this;
    }

    message_queue::~message_queue ()
    {
        if (m_listener)
        {
            m_listener->notify (m_id, nullptr, notification_flag::Closed);
        }
    }

    queue_id_type message_queue::getQID () const
    {
        return m_id;
    }

    status_code message_queue::push (Message::upointer_type && msg)
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
            m_listener->notify (m_id, this, notification_flag::NewData);
        }
	return ExitStatus::Success;
    }

    Message::upointer_type message_queue::pop ()
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

    status_code message_queue::set_listener (listener & l)
    {
        lock_type guard (m_mutex);
        if (m_listener)
        {
            return ExitStatus::AlreadyExist;
        }

        m_listener = &l;
        if (!m_queue.empty ())
        {
            m_listener->notify (m_id, this, notification_flag::NewData);
        }
        return ExitStatus::Success;
    }

    void message_queue::clear_listener ()
    {
        lock_type guard (m_mutex);
        m_listener = nullptr;
    }
} /* namespace mqmx */
