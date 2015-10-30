#include <mqmx/message_queue.h>
#include <cassert>

namespace mqmx
{
    message_queue::message_queue (const queue_id_type ID) noexcept
        : _id (ID)
        , _mutex ()
        , _queue ()
        , _listener (nullptr)
    {
    }

    queue_id_type message_queue::get_id () const
    {
        return _id;
    }

    status_code message_queue::push (message_ptr_type && msg)
    {
        if (msg.get () == nullptr)
        {
            return ExitStatus::InvalidArgument;
        }

        lock_type guard (_mutex);
        if ((_id == message::undefined_qid) ||
            (_id != msg->get_qid ()))
        {
            return ExitStatus::NotSupported;
        }

        _queue.push_back (std::move (msg));
        if (_listener)
        {
            _listener->notify (_id, this, MQNotification::NewMessage);
        }
        return ExitStatus::Success;
    }

    message_queue::message_ptr_type message_queue::pop ()
    {
        message_ptr_type msg;
        lock_type guard (_mutex);
        if ((_id != message::undefined_qid) && !_queue.empty ())
        {
            msg = std::move (_queue.front ());
            _queue.pop_front ();
        }
        return std::move (msg);
    }

    status_code message_queue::set_listener (listener & l)
    {
        lock_type guard (_mutex);
        if (_listener)
        {
            return ExitStatus::AlreadyExist;
        }

        _listener = &l;
        if (!_queue.empty ())
        {
            _listener->notify (_id, this, MQNotification::NewMessage);
        }
        return ExitStatus::Success;
    }

    void message_queue::clear_listener ()
    {
        lock_type guard (_mutex);
        _listener = nullptr;
    }
} /* namespace mqmx */
