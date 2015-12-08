#pragma once

#include <mutex>
#include <thread>
#include <condition_variable>

namespace BBC_pkg
{
    typedef std::thread             oam_thread_type;
    typedef std::condition_variable oam_condvar_type;

    /**
     * Auxiliary class for synchronization.
     */
    class CompletionSignal
    {
    public:
        typedef std::mutex                        mutex_type;
        typedef std::unique_lock<mutex_type>      lock_type;
        typedef oam_condvar_type                  condvar_type;
        typedef std::unique_ptr<CompletionSignal> upointer_type;

    private:
        mutex_type   _signal_mutex;
        condvar_type _signal_condition;
        bool         _signal_flag;

    public:
        CompletionSignal ()
            : _signal_mutex ()
            , _signal_condition ()
            , _signal_flag (false)
        { }

        void post ()
        {
            lock_type guard (_signal_mutex);
            _signal_condition.notify_one ();
            _signal_flag = true;
        }

        void wait ()
        {
            lock_type guard (_signal_mutex);
            _signal_condition.wait (guard, [&]{ return _signal_flag; });
        }
    };
} /* namespace BBC_pkg */
