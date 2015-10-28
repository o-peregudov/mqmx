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

    status_code message_queue_pool::wait ()
    {
        lock_type guard (_mutex);
        _condition.wait (guard, [&]{ return (_has_messages || _terminated); });
        const status_code retCode (_terminated
                                   ? ExitStatus::Finished
                                   : ExitStatus::Success);
        _has_messages = false;
        _terminated = false;
        return retCode;
    }

    status_code message_queue_pool::wait_for (
        const std::chrono::high_resolution_clock::duration & rel_time)
    {
        lock_type guard (_mutex);
        if (_condition.wait_for (guard, rel_time,
                                 [&]{ return (_has_messages || _terminated); }))
        {
            const status_code retCode (_terminated
                                       ? ExitStatus::Finished
                                       : ExitStatus::Success);
            _has_messages = false;
            _terminated = false;
            return retCode;
        }
        return ExitStatus::Timeout;
    }

    status_code message_queue_pool::wait_until (
        const std::chrono::high_resolution_clock::time_point & abs_time)
    {
        lock_type guard (_mutex);
        if (_condition.wait_until (guard, abs_time,
                                   [&]{ return (_has_messages || _terminated); }))
        {
            const status_code retCode (_terminated
                                       ? ExitStatus::Finished
                                       : ExitStatus::Success);
            _has_messages = false;
            _terminated = false;
            return retCode;
        }
        return ExitStatus::Timeout;
    }

    void message_queue_pool::dispatch ()
    {
        lock_type rwguard (_rwmutex);
        _rwcondition.wait (rwguard, [&]{ return (_nwriters == 0); });
        ++_nreaders;
        rwguard.unlock ();

        _dispatch ();

        rwguard.lock ();
        if (--_nreaders == 0)
        {
            _rwcondition.notify_one ();
        }
    }

    void message_queue_pool::_dispatch ()
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
} /* namespace mqmx */
