#include <mqmx/work_queue.h>
#include <algorithm>

namespace mqmx
{
    const work_queue::client_id_type work_queue::INVALID_CLIENT_ID =
        static_cast<work_queue::client_id_type> (-1);
    const work_queue::work_id_type work_queue::INVALID_WORK_ID =
        static_cast<work_queue::work_id_type> (-1);
    const work_queue::duration_type work_queue::RUN_ONCE =
        work_queue::duration_type ();

    work_queue::wq_item::wq_item ()
        : time_point ()
        , client_id (INVALID_CLIENT_ID)
        , work ()
        , period ()
    { }

    work_queue::wq_item::wq_item (
        const work_queue::time_point_type & tpoint,
        const work_queue::client_id_type clientid,
        const work_queue::work_pointer_type & pwork,
        const work_queue::duration_type & tperiod)
        : time_point (tpoint)
        , client_id (clientid)
        , work (pwork)
        , period (tperiod)
    { }

    work_queue::work_queue ()
        : work_queue (dont_start_worker ())
    {
        start_worker ();
    }

    work_queue::work_queue (const dont_start_worker)
        : _next_work_id (INVALID_WORK_ID)
        , _next_client_id (INVALID_CLIENT_ID)
        , _mutex ()
        , _container_change_condition ()
        , _wq_item_container ()
        , _container_change_flag (false)
        , _worker_stopped_flag (true)
        , _worker ()
    { }

    work_queue::~work_queue ()
    {
        kill_worker ();
    }

    std::pair<status_code, work_queue::work_id_type> work_queue::post_work (
        work_queue::lock_type & guard,
        work_queue::wq_item item)
    {
        if (_worker_stopped_flag)
            return std::make_pair (ExitStatus::NotAllowed, INVALID_WORK_ID);

        if (++_next_work_id == INVALID_WORK_ID)
            ++_next_work_id;

        _wq_item_container.push_back (std::make_pair (item, _next_work_id));
        std::push_heap (std::begin (_wq_item_container),
                        std::end (_wq_item_container),
                        record_compare ());

        signal_container_change (guard);
        return std::make_pair(ExitStatus::Success, _next_work_id);
    }

    bool work_queue::is_idle () const
    {
        lock_type guard (_mutex);

        return is_container_empty (guard);
    }

    std::pair<status_code, work_queue::work_id_type> work_queue::schedule_work (
        const client_id_type client_id,
        const work_pointer_type & work,
        const time_point_type & start_time,
        const duration_type & repeat_period)
    {
        if ((client_id == INVALID_CLIENT_ID) || !work)
            return std::make_pair (ExitStatus::InvalidArgument, INVALID_WORK_ID);

        time_point_type stime =
            is_time_point_empty (start_time) ? get_current_time_point () : start_time;

        lock_type guard (_mutex);

        return post_work (guard, {stime, client_id, work, repeat_period});
    }

    bool work_queue::signal_worker_to_stop ()
    {
        lock_type guard (_mutex);

        if (_worker_stopped_flag)
            return false;

        _wq_item_container.clear ();

        status_code sc = ExitStatus::Success;
        work_id_type work_id = INVALID_WORK_ID;
        std::tie (sc, work_id) = post_work (guard, wq_item ());
        if (sc == ExitStatus::Success)
        {
            _worker_stopped_flag = true;
            return true;
        }
        return false;
    }

    void work_queue::reset_container_change_flag (work_queue::lock_type & /*guard*/)
    {
        _container_change_flag = false;
    }

    void work_queue::signal_container_change (work_queue::lock_type & /*guard*/)
    {
        _container_change_flag = true;
        _container_change_condition.notify_one ();
    }

    status_code work_queue::start_worker ()
    {
        lock_type guard (_mutex);

        if (!_worker_stopped_flag)
            return ExitStatus::NotAllowed;

        std::thread wrk ([this]{ worker (); });
        std::swap (wrk, _worker);

        _worker_stopped_flag = false;
        return ExitStatus::Success;
    }

    status_code work_queue::kill_worker ()
    {
        if (signal_worker_to_stop () && _worker.joinable ())
        {
            _worker.join ();
            return ExitStatus::Success;
        }
        return ExitStatus::NotAllowed;
    }

    void work_queue::make_heap_and_notify_worker (work_queue::lock_type & guard)
    {
        std::make_heap (std::begin (_wq_item_container),
                        std::end (_wq_item_container),
                        record_compare ());
        signal_container_change (guard);
    }

    bool work_queue::wq_item_find_and_replace (
        lock_type & /*guard*/, const work_id_type work_id, const wq_item & new_item)
    {
        for (auto & elem : _wq_item_container)
        {
            if (elem.second == work_id)
            {
                elem.first = new_item;
                return true;
            }
        }
        return false;
    }

    status_code work_queue::update_work (
        const work_id_type work_id,
        const client_id_type client_id,
        const work_pointer_type & work,
        const time_point_type & start_time,
        const duration_type & repeat_period)
    {
        if ((client_id == INVALID_CLIENT_ID) || !work)
            return ExitStatus::InvalidArgument;

        lock_type guard (_mutex);

        if (_worker_stopped_flag)
            return ExitStatus::NotAllowed;

        if (wq_item_find_and_replace (
                guard, work_id, {start_time, client_id, work, repeat_period}))
        {
            make_heap_and_notify_worker (guard);
            return ExitStatus::Success;
        }
        return ExitStatus::NotFound;
    }

    bool work_queue::wq_item_find_and_remove (
        lock_type & /*guard*/, const work_id_type work_id)
    {
        for (auto & elem : _wq_item_container)
        {
            if (elem.second == work_id)
            {
                std::swap (elem, _wq_item_container.back ());
                _wq_item_container.pop_back ();
                return true;
            }
        }
        return false;
    }

    status_code work_queue::cancel_work (const work_queue::work_id_type work_id)
    {
        lock_type guard (_mutex);

        if (_worker_stopped_flag)
            return ExitStatus::NotAllowed;

        if (wq_item_find_and_remove (guard, work_id))
        {
            make_heap_and_notify_worker (guard);
            return ExitStatus::Success;
        }
        return ExitStatus::NotFound;
    }

    work_queue::time_point_type work_queue::get_nearest_time_point () const
    {
        lock_type guard (_mutex);
        return get_nearest_time_point (guard);
    }

    work_queue::time_point_type work_queue::get_nearest_time_point (
        lock_type & guard) const
    {
        if (is_container_empty (guard))
            return get_empty_time_point ();

        return _wq_item_container.front ().first.time_point;
    }

    void work_queue::signal_going_to_idle (work_queue::lock_type & /*guard*/)
    {
        /* derived class can use this */
    }

    bool work_queue::wait_for_some_work (work_queue::lock_type & guard)
    {
        if (is_container_empty (guard))
            signal_going_to_idle (guard);

        _container_change_condition.wait (guard, [&]{
                return !is_container_empty (guard);
            });
        return static_cast<bool> (_wq_item_container.front ().first.work);
    }

    bool work_queue::get_container_change_flag (work_queue::lock_type & /*guard*/) const
    {
        return _container_change_flag;
    }

    bool work_queue::wait_for_time_point (
        work_queue::lock_type & guard, const work_queue::time_point_type & timepoint)
    {
        if (_container_change_condition.wait_until (guard, timepoint, [&]{
                    return get_container_change_flag (guard);
                }))
        {
            reset_container_change_flag (guard);
            return false;
        }
        return true;
    }

    work_queue::time_point_type work_queue::execute_work (
        lock_type & /*guard*/, const work_queue::record_type & rec)
    {
        auto rescheduled_work_time_point = get_empty_time_point ();
        try
        {
            const bool rescheduling_needed =
                rec.first.work (rec.second) && (0 < rec.first.period.count ());
            if (rescheduling_needed)
                rescheduled_work_time_point = rec.first.time_point + rec.first.period;
        }
        catch (...)
        {
        }
        return rescheduled_work_time_point;
    }

    work_queue::time_point_type work_queue::get_empty_time_point () const
    {
        return time_point_type ();
    }

    void work_queue::worker ()
    {
        lock_type guard (_mutex);
        for (;;)
        {
            if (!wait_for_some_work (guard))
            {
                // got termination notification - exit thread
                _wq_item_container.clear ();
                break;
            }

            if (!wait_for_time_point (guard, _wq_item_container.front ().first.time_point) ||
                is_container_empty (guard))
            {
                // timer queue has been changed - we have to restart the loop
                continue;
            }

            std::pop_heap (std::begin (_wq_item_container), std::end (_wq_item_container),
                           record_compare ());
            record_type item = std::move (_wq_item_container.back ());
            _wq_item_container.pop_back ();

            const auto rescheduled_work_time_point = execute_work (guard, item);
            if (!is_time_point_empty (rescheduled_work_time_point))
            {
                item.first.time_point = rescheduled_work_time_point;
                _wq_item_container.push_back (std::move (item));
                std::push_heap (std::begin (_wq_item_container),
                                std::end (_wq_item_container),
                                record_compare ());
            }
        }
    }

    bool work_queue::wq_item_find_and_remove_all (
        lock_type & /*guard*/, const client_id_type client_id)
    {
        auto is_client_predicate = [client_id](const record_type & r){
            return (r.first.client_id == client_id);
        };

        container_type::iterator new_end =
            std::remove_if (std::begin (_wq_item_container),
                            std::end (_wq_item_container),
                            is_client_predicate);

        if (new_end != std::end (_wq_item_container))
        {
            _wq_item_container.erase (new_end, _wq_item_container.end ());
            return true;
        }
        return false;
    }

    status_code work_queue::cancel_client_works (const work_queue::client_id_type client_id)
    {
        lock_type guard (_mutex);

        if (_worker_stopped_flag)
            return ExitStatus::NotAllowed;

        if (wq_item_find_and_remove_all (guard, client_id))
        {
            make_heap_and_notify_worker (guard);
            return ExitStatus::Success;
        }
        return ExitStatus::NotFound;
    }

    work_queue::client_id_type work_queue::get_client_id ()
    {
        lock_type guard (_mutex);

        if (++_next_client_id == INVALID_CLIENT_ID)
            ++_next_client_id;

        return _next_client_id;
    }

    work_queue::time_point_type work_queue::get_current_time_point () const
    {
        return clock_type::now ();
    }

    bool work_queue::is_container_empty (work_queue::lock_type & /*guard*/) const
    {
        return _wq_item_container.empty ();
    }
} /* namespace mqmx */
