#include <mqmx/message_queue.h>
#include <cassert>

namespace mqmx
{
    message_queue::message_queue (const queue_id_type ID)
        : _id (ID)
        , _mutex ()
        , _queue ()
        , _listener (nullptr)
    {
    }

    message_queue::message_queue (message_queue && o)
        : _id (message::undefined_qid)
        , _mutex ()
        , _queue ()
        , _listener (nullptr)
    {
        lock_type guard (o._mutex);
        std::swap (_queue, o._queue);
        std::swap (_id, o._id);
        std::swap (_listener, o._listener);
        if (_listener)
        {
            auto cplistener (_listener);
            _listener = nullptr;

            cplistener->notify (_id, &o, notification_flag::detached);
        }
    }

    message_queue & message_queue::operator = (message_queue && o)
    {
        if (this != &o)
        {
            std::lock (_mutex, o._mutex);
            lock_type guard_this (_mutex, std::adopt_lock_t ());
            lock_type guard_o (o._mutex, std::adopt_lock_t ());
            if (_listener)
            {
                _listener->notify (_id, this, notification_flag::detached);
                _listener = nullptr;
            }
            _queue.clear ();
            _id = message::undefined_qid;
            std::swap (_queue, o._queue);
            std::swap (_id, o._id);
            std::swap (_listener, o._listener);
            if (_listener)
            {
                auto cplistener (_listener);
                _listener = nullptr;

                cplistener->notify (_id, &o, notification_flag::detached);
            }
        }
        return *this;
    }

    message_queue::~message_queue ()
    {
        if (_listener)
        {
            _listener->notify (_id, nullptr, notification_flag::closed);
        }
    }

    queue_id_type message_queue::get_qid () const
    {
        return _id;
    }

    status_code message_queue::push (message::upointer_type && msg)
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
        if (_listener && (_queue.size () == 1))
        {
            /* only first message will be reported */
            _listener->notify (_id, this, notification_flag::data);
        }
	return ExitStatus::Success;
    }

    message::upointer_type message_queue::pop ()
    {
        message::upointer_type msg;
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
            _listener->notify (_id, this, notification_flag::data);
        }
        return ExitStatus::Success;
    }

    void message_queue::clear_listener ()
    {
        lock_type guard (_mutex);
        _listener = nullptr;
    }
} /* namespace mqmx */
