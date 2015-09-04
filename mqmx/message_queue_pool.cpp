#include <mqmx/message_queue_pool.h>
#include <cassert>

namespace mqmx
{
    void message_queue_pool::terminate ()
    {
        lock_type guard (_mutex);
        _terminated = true;
        _condition.notify_one ();
    }

    int message_queue_pool::wait ()
    {
        lock_type guard (_mutex);
        _condition.wait (guard, [&]{ return (_has_messages || _terminated); });
        const bool sterminated = _terminated;
        _has_messages = false;
        _terminated = false;
        return (sterminated ? ExitStatus::Finished : ExitStatus::Success);
    }

    int message_queue_pool::wait_for (
        const std::chrono::high_resolution_clock::duration & rel_time)
    {
        lock_type guard (_mutex);
        if (_condition.wait_for (guard, rel_time,
				 [&]{ return (_has_messages || _terminated); }))
        {
            const bool sterminated = _terminated;
            _has_messages = false;
            _terminated = false;
	    return (sterminated ? ExitStatus::Finished : ExitStatus::Success);
        }
        return -ExitStatus::Timeout;
    }

    int message_queue_pool::wait_until (
        const std::chrono::high_resolution_clock::time_point & abs_time)
    {
        lock_type guard (_mutex);
        if (_condition.wait_until (guard, abs_time,
				   [&]{ return (_has_messages || _terminated); }))
        {
            const bool sterminated = _terminated;
            _has_messages = false;
            _terminated = false;
	    return (sterminated ? ExitStatus::Finished : ExitStatus::Success);
        }
        return ExitStatus::Timeout;
    }

    void message_queue_pool::dispatch ()
    {
        lock_type rwguard (_rwmutex);
        _rwcondition.wait (
            rwguard, [&]{ return (_nwriters == 0); });
        ++_nreaders;
        rwguard.unlock ();

        _dispatch_unlocked ();

        rwguard.lock ();
        if (--_nreaders == 0)
        {
            _rwcondition.notify_one ();
        }
    }

    void message_queue_pool::_dispatch_unlocked ()
    {
        counter_container_type snapshot_counter (_queue.size (), 0);
        std::swap (_counter, snapshot_counter);
        for (size_t ix = 0; ix < _queue.size (); ++ix)
        {
            for (; 0 < snapshot_counter[ix]; --snapshot_counter[ix])
            {
                message_queue::message_ptr_type msg = _queue[ix].pop ();
                if (msg)
                {
                    assert (_handler[ix]);
                    (*_handler[ix])(std::move (msg));
                }
            }
        }
    }

    int message_queue_pool::_add (const size_t qid, handler_ptr_type && h)
    {
        lock_type guard (_mutex);
        if (_queue.size () <= qid)
        {
            _queue.resize (qid + 1);
            _queue[qid] = message_queue (qid);

            _counter.resize (qid + 1, 0);

            _handler.resize (qid + 1);
            _handler[qid] = std::move (h);

            return ExitStatus::Success;
        }
        return ExitStatus::AlreadyExist;
    }

    int message_queue_pool::_push (message_queue::message_ptr_type && msg)
    {
        const size_t qid = msg->get_qid ();
        int retCode = ((qid < _queue.size ())
                       ? _queue[qid].push (std::move (msg))
                       : ExitStatus::NotFound);
        if (retCode == ExitStatus::Success)
        {
            lock_type guard (_mutex);
            ++(_counter[qid]);
            _has_messages = true;
            _condition.notify_one ();
        }
        return retCode;
    }

    message_queue_pool::message_queue_pool ()
        : _mutex ()
        , _condition ()
        , _has_messages (false)
        , _terminated (false)
        , _queue ()
        , _counter ()
        , _rwmutex ()
        , _nreaders (0)
        , _nwriters (0)
        , _rwcondition ()
    {
    }

    message_queue_pool::~message_queue_pool ()
    {
    }

    int message_queue_pool::push (message_queue::message_ptr_type && msg)
    {
        if ((msg.get () == nullptr) ||
            (msg->get_qid () == message::undefined_qid))
        {
            return ExitStatus::InvalidArgument;
        }

        lock_type rwguard (_rwmutex);
        _rwcondition.wait (
            rwguard, [&]{ return (_nwriters == 0); });
        ++_nreaders;
        rwguard.unlock ();

        int retCode = _push (std::move (msg));

        rwguard.lock ();
        if (--_nreaders == 0)
        {
            _rwcondition.notify_one ();
        }
        return retCode;
    }

    int message_queue_pool::add_queue (const size_t qid, handler_ptr_type && handler)
    {
        if ((qid == message::undefined_qid) ||
            (handler.get () == nullptr))
        {
            return ExitStatus::InvalidArgument;
        }

        lock_type rwguard (_rwmutex);
        _rwcondition.wait (
            rwguard, [&]{ return (_nwriters == 0) && (_nreaders == 0); });
        ++_nwriters;
        rwguard.unlock ();

        int retCode = _add (qid, std::move (handler));

        rwguard.lock ();
        if (--_nwriters == 0)
        {
            _rwcondition.notify_one ();
        }
        return retCode;
    }
} /* namespace mqmx */
