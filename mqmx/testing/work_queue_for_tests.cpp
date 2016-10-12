#include <mqmx/testing/work_queue_for_tests.h>

namespace mqmx
{
namespace testing
{
    work_queue_for_tests::work_queue_for_tests (const sync_function_type & sync_func)
        : work_queue (work_queue::dont_start_worker ())
        , _current_time_point (work_queue::clock_type::now ())
        , _time_forwarding_flag (false)
        , _time_forwarding_completed ()
        , _time_forwarding_client_id (work_queue::INVALID_CLIENT_ID)
        , _sync_function (sync_func)
    {
        _time_forwarding_client_id = get_client_id ();
        start_worker ();
    }

    work_queue_for_tests::~work_queue_for_tests ()
    {
        kill_worker ();
    }

    bool work_queue_for_tests::wait_for_time_forwarding_completion (
        const work_queue::time_point_type final_time_point)
    {
        status_code ec = ExitStatus::Success;
        work_queue::work_id_type fwork_id = work_queue::INVALID_WORK_ID;
        std::tie (ec, fwork_id) = schedule_work (
            _time_forwarding_client_id,
            [this](const work_queue::work_id_type) {
                _time_forwarding_flag = true;
                return false;
            },
            final_time_point);

        if (ec == ExitStatus::Success)
        {
            _time_forwarding_completed.wait ();
            return true;
        }
        return false;
    }

    bool work_queue_for_tests::forward_time (
        const work_queue::time_point_type & tp, const bool wait_for_completion)
    {
        work_queue::lock_type guard (_mutex);
        work_queue::time_point_type dst_time_point = (_current_time_point = tp);

        if (wait_for_completion)
        {
            guard.unlock ();
            return wait_for_time_forwarding_completion (dst_time_point);
        }

        signal_container_change (guard);
        return true;
    }

    bool work_queue_for_tests::forward_time (
        const work_queue::duration_type & rt, const bool wait_for_completion)
    {
        work_queue::lock_type guard (_mutex);
        work_queue::time_point_type dst_time_point = (_current_time_point += rt);

        if (wait_for_completion)
        {
            guard.unlock ();
            return wait_for_time_forwarding_completion (dst_time_point);
        }

        signal_container_change (guard);
        return true;
    }

    bool work_queue_for_tests::forward_time (const bool wait_for_completion)
    {
        const work_queue::time_point_type nearest_time_point =
            get_nearest_time_point ();
        return forward_time (is_time_point_empty (nearest_time_point)
                             ? get_current_time_point ()
                             : nearest_time_point,
                             wait_for_completion);
    }

    work_queue::time_point_type work_queue_for_tests::get_current_time_point () const
    {
        work_queue::lock_type guard (_mutex);
        return _current_time_point;
    }

    bool work_queue_for_tests::wait_for_time_point (
        work_queue::lock_type & guard, const work_queue::time_point_type & timepoint)
    {
        if (timepoint <= _current_time_point)
            return true;

        _container_change_condition.wait (guard, [&]{
                return get_container_change_flag (guard);
            });
        reset_container_change_flag (guard);
        return false;
    }

    work_queue::time_point_type work_queue_for_tests::execute_work (
        work_queue::lock_type & guard, const work_queue::record_type & rec)
    {
        const auto result = work_queue::execute_work (guard, rec);
        if (_time_forwarding_flag)
        {
            _time_forwarding_flag = false;
            const auto next_tp = get_nearest_time_point (guard);
            if (is_time_point_empty (next_tp) || (_current_time_point < next_tp))
                _time_forwarding_completed.post ();
            else
                return _current_time_point;
        }
        else if (_sync_function)
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
} /* namespace testing */
} /* namespace mqmx */
