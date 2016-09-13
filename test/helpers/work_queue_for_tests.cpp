#include "test/helpers/work_queue_for_tests.h"

namespace helpers
{
    work_queue_for_tests::work_queue_for_tests (const sync_function_type & sync_func)
        : mqmx::work_queue ()
        , _current_time_point (mqmx::work_queue::clock_type::now ())
        , _time_point_changed (false)
        , _time_forwarding_condition ()
        , _time_forwarding_completed (false)
        , _sync_function (sync_func)
    { }

    work_queue_for_tests::~work_queue_for_tests ()
    {
        kill_worker ();
    }

    mqmx::work_queue::time_point_type
    work_queue_for_tests::forward_time (const mqmx::work_queue::time_point_type & tp,
                                        const bool wait_for_completion)
    {
        mqmx::work_queue::lock_type guard (_mutex);
        const mqmx::work_queue::time_point_type old_time_point = _current_time_point;

        _current_time_point = tp;

        _time_point_changed = true;
        _container_change_condition.notify_one ();

        if (wait_for_completion)
        {
            _time_forwarding_completed = false;
            _time_forwarding_condition.wait (guard, [&]{
                    return (_time_forwarding_completed || is_container_empty (guard));
                });
        }
        return old_time_point;
    }

    mqmx::work_queue::time_point_type
    work_queue_for_tests::forward_time (const mqmx::work_queue::duration_type & rt,
                                        const bool wait_for_completion)
    {
        mqmx::work_queue::lock_type guard (_mutex);
        const mqmx::work_queue::time_point_type old_time_point = _current_time_point;

        _current_time_point += rt;

        _time_point_changed = true;
        _container_change_condition.notify_one ();

        if (wait_for_completion)
        {
            _time_forwarding_completed = false;
            _time_forwarding_condition.wait (guard, [&]{
                    return (_time_forwarding_completed || is_container_empty (guard));
                });
        }
        return old_time_point;
    }

    void work_queue_for_tests::forward_time (const bool wait_for_completion)
    {
        const mqmx::work_queue::time_point_type nearest_time_point =
            get_nearest_time_point ();

        if (mqmx::is_time_point_empty (nearest_time_point))
            forward_time (get_current_time_point (), wait_for_completion);
        else
            forward_time (nearest_time_point, wait_for_completion);
    }

    mqmx::work_queue::time_point_type work_queue_for_tests::get_current_time_point () const
    {
        mqmx::work_queue::lock_type guard (_mutex);
        return _current_time_point;
    }

    void work_queue_for_tests::signal_going_to_idle (mqmx::work_queue::lock_type &)
    {
        _time_forwarding_completed = true;
        _time_forwarding_condition.notify_one ();
    }

    bool work_queue_for_tests::wait_for_time_point (
        mqmx::work_queue::lock_type & guard,
        const mqmx::work_queue::time_point_type & timepoint)
    {
        if (timepoint <= _current_time_point)
            return true;

        signal_going_to_idle (guard);

        _container_change_condition.wait (guard, [&]{
                return (_time_point_changed || get_container_change_flag (guard));
            });

        reset_container_change_flag (guard);
        _time_point_changed = false;
        return false;
    }

    mqmx::work_queue::time_point_type work_queue_for_tests::execute_work (
        mqmx::work_queue::lock_type & guard,
        const mqmx::work_queue::record_type & rec)
    {
        const auto result = mqmx::work_queue::execute_work (guard, rec);
        if (_sync_function)
        {
            guard.unlock ();
            try
            {
                _sync_function (rec.second, rec.first.time_point);
            }
            catch (...)
            {
            }
            guard.lock ();
        }
        return result;
    }
} /* namespace helpers */
