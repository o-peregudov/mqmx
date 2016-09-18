#pragma once

#include <vector>
#include <memory>
#include <thread>
#include <functional>

#include <crs/mutex.h>
#include <crs/condition_variable.h>

#include <mqmx/libexport.h>
#include <mqmx/types.h>
#include <mqmx/wait_time_provider.h>

namespace mqmx
{
    /**
     * \brief Generic work queue (WQ).
     *
     * Main idea behind this implementation is asynchronous execution of
     * user work(s) either immediately or at specified moment(s) of time.
     * Periodic works are also supported.
     *
     * For better testability class also provides some interface (virtual and
     * protected members) for derived classes. This interface mainly covers
     * the places where waiting for some time point is performed. So derived
     * class might have a better control of the internal worker.
     *
     * \note Very short timeout values (usually less than some milliseconds)
     *       cannot be guaranteed. Actual precision was not estimated and
     *       depends on the system.
     */
    class MQMX_EXPORT work_queue
    {
        work_queue (const work_queue &) = delete;
        work_queue & operator = (const work_queue &) = delete;

    public:
        using mutex_type        = crs::mutex_type;
        using lock_type         = crs::lock_type;
        using condvar_type      = crs::condvar_type;
        using thread_type       = std::thread;
        using clock_type        = wait_time_provider::clock_type;
        using time_point_type   = clock_type::time_point;
        using duration_type     = clock_type::duration;
        using upointer_type     = std::unique_ptr<work_queue>;
        using client_id_type    = unsigned long;
        using work_id_type      = unsigned long;
        using work_pointer_type = std::function<bool (const work_id_type)>;

        static const client_id_type INVALID_CLIENT_ID; ///< invalid (unused) client ID
        static const work_id_type   INVALID_WORK_ID;   ///< invalid (unused) work ID
        static const duration_type  RUN_ONCE;          ///< empty (zero) period

    protected:
        /**
         * \brief Data structure used by internal WQ implementation.
         */
        struct wq_item
        {
            time_point_type   time_point; ///< next work execution time point
            client_id_type    client_id;  ///< ID of the work's owner
            work_pointer_type work;       ///< pointer to a work function
            duration_type     period;     ///< invocation repetition period

            wq_item ();
            wq_item (const time_point_type & tpoint,
                     const client_id_type clientid,
                     const work_pointer_type & pwork,
                     const duration_type & tperiod);
            wq_item (wq_item &&) = default;
            wq_item (const wq_item &) = default;
            wq_item & operator = (wq_item &&) = default;
            wq_item & operator = (const wq_item &) = default;
        };

        typedef std::pair<wq_item, work_id_type> record_type;
        typedef std::vector<record_type>         container_type;

        /**
         * \brief Custom comparator used by internal WQ implementation.
         *
         * Guarantees natural ordering of work items.
         */
        struct record_compare
        {
            bool operator () (const record_type & a, const record_type & b) const
            {
                return b.first.time_point < a.first.time_point;
            }
        };

    public:
        /**
         * \brief Default constructor.
         *
         * Initializes all internal data members and starts internal worker thread.
         */
        work_queue ();

        /**
         * \brief Destructor.
         *
         * Terminates internal worker thread and clears all internal data members.
         *
         * \remark Despite the fact internal worker thread is terminated
         *         automatically it is always a good idea to stop it
         *         explicitly before destruction of instance of WQ
         */
        virtual ~work_queue ();

        /**
         * \brief Checks, whether worker thread is waiting for any work to be
         * scheduled (i.e. internal queue is empty) or not.
         */
        bool is_idle () const;

        /**
         * \brief Terminates worker thread immediately, but gently.
         *
         * All works, which were present in queue, but were not processed will
         * be discarded.
         *
         * \note
         *  - you cannot put new works into queue after this call;
         *  - this is kind of final call, because there are no possibility to
         *    restart (recreate) internal worker thread after this call.
         */
        void kill_worker ();

        /**
         * \brief Schedule new work for asynchronous execution.
         *
         * \param client_id is an ID, aimed to group work items which belongs to
         *        some particular client
         * \param work is a pointer to function, that should be executed
         * \param start_time is a time point, when work execution should be
         *        triggered for the first time
         * \param repeat_period is a timeout, that is used to periodically
         *        re-trigger work execution since first trigger (it can be set
         *        to RUN_ONCE if periodic triggering is not needed)
         *
         * \retval ExitStatus::NotAllowed      if worker thread is terminated
         * \retval ExitStatus::InvalidArgument if work pointer is null
         * \retval ExitStatus::Success         in case of success
         */
        std::pair<status_code, work_id_type> schedule_work (
            const client_id_type client_id,
            const work_pointer_type & work,
            const time_point_type & start_time = time_point_type (),
            const duration_type & repeat_period = RUN_ONCE);

        /**
         * \brief Update work with given ID.
         *
         * This methods provides the possibility to change work item.
         * Existing record will be replaced with the new one, and only work ID
         * will be preserved.
         *
         * \param work_id is the ID of already posted work to be altered
         * \param client_id is an ID, aimed to group work items which belongs to
         *        some particular client
         * \param work is a pointer to function, that should be executed
         * \param start_time is a time point, when work should be triggered
         *        for the first time
         * \param repeat_period is a timeout, that is used to periodically
         *        re-trigger work execution since first trigger (it can be set
         *        to RUN_ONCE if periodic triggering is not needed)
         *
         * \retval ExitStatus::NotAllowed      if worker thread is terminated
         * \retval ExitStatus::InvalidArgument if work pointer is null
         * \retval ExitStatus::NotFound        if work with given ID is not in
         *                                     the queue
         * \retval ExitStatus::Success         in case of success
         */
        status_code update_work (const work_id_type work_id,
                                 const client_id_type client_id,
                                 const work_pointer_type & work,
                                 const time_point_type & start_time,
                                 const duration_type & repeat_period);

        /**
         * \brief Cancel work with given ID.
         *
         * \param work_id is the ID of already posted work to be canceled
         *
         * \retval ExitStatus::NotAllowed if worker thread is terminated
         * \retval ExitStatus::NotFound   if work with given ID is not in the queue
         * \retval ExitStatus::Success    in case of success
         */
        status_code cancel_work (const work_id_type work_id);

        /**
         * \brief Cancel all work items, which belongs to specified owner.
         *
         * \retval ExitStatus::NotAllowed if worker thread is terminated
         * \retval ExitStatus::NotFound   if no events were listed in parameter
         * \retval ExitStatus::Success    if events were removed from the queue
         */
        status_code cancel_client_works (const client_id_type client_id);

        /**
         * \brief Get unique client ID.
         *
         * Unique client ID is needed for grouping work item by client/owner.
         * User have to get it's own unique ID before scheduling any work for
         * execution.
         */
        client_id_type get_client_id ();

        /**
         * \brief Get nearest time point.
         *
         * \returns time point of nearest work item to be executed or empty time
         *          point (set to epoch) if there are no events scheduled
         *
         * \note This method is in general needed for tests.
         */
        time_point_type get_nearest_time_point () const;

        /**
         * \brief Get current time point.
         *
         * \returns clock_type::now ()
         */
        virtual time_point_type get_current_time_point () const;

    protected:
        /**
         * \brief Cleares internal container change flag.
         *
         * \attention Method is called with main mutex acquired.
         */
        void reset_container_change_flag (lock_type & guard);

        /**
         * \brief Returns internal container change flag.
         *
         * \attention Method is called with main mutex acquired.
         */
        bool get_container_change_flag (lock_type & guard) const;

        /**
         * \brief Checks, whether internal container is empty.
         *
         * \attention Method is called with main mutex acquired.
         */
        bool is_container_empty (lock_type & guard) const;

        /**
         * \brief Signals entering waiting state without timeout.
         *
         * In current implementation worker has the only wait state without
         * timeout and it is waiting for some work to be scheduled (internal
         * container is empty). Derived class can override this method, but
         * it should also call this method before entering any wait state
         * without timeout.
         *
         * \attention Method is called with main mutex acquired.
         */
        virtual void signal_going_to_idle (lock_type & guard);

        /**
         * \brief Waits for the next time point.
         *
         * Function causes the worker thread to block until the main conditional
         * variable is notified (about changes of internal container) or specified
         * time is reached (and a scheduled work with given time point should be
         * executed).
         *
         * In current implementation this operation depends on system clocks API,
         * yet derived classes may use some different criteria for waiting.
         *
         * \param guard acquired lock for main mutex
         * \param time_point a closest work's time point
         *
         * \retval true  if time point was reaced
         * \retval false if internal container has been changed and worker
         *               should restart waiting
         *
         * \attention Method is called with main mutex acquired.
         */
        virtual bool wait_for_time_point (
            lock_type & guard, const time_point_type & time_point);

        /**
         * \brief Executes user work.
         *
         * After work execution this method checks conditions for work
         * rescheduling and calculates next time point, when this work should be
         * executed once again. Usually next time point is calculated as
         * <i>current-time-point + repeat-period</i>.
         *
         * \param guard acquired lock for main mutex
         * \param record internal WQ item record
         *
         * \returns time point when next work execution should be scheduled
         *          or empty time point in case work rescheduling is not
         *          needed
         *
         * \attention Method is called with main mutex acquired.
         */
        virtual time_point_type execute_work (
            lock_type & guard, const record_type & record);

    private:
        std::pair<status_code, work_id_type> post_work (lock_type &, wq_item);
        time_point_type get_empty_time_point () const;

        bool wait_for_some_work (lock_type &);
        void signal_container_change (lock_type &);
        void make_heap_and_notify_worker (lock_type &);
        bool wq_item_find_and_replace (
            lock_type &, const work_id_type, const wq_item & new_item);
        bool wq_item_find_and_remove (lock_type &, const work_id_type);
        bool wq_item_find_and_remove_all (lock_type &, const client_id_type);
        bool signal_worker_to_stop ();
        void worker ();

        work_id_type       _next_work_id;
        client_id_type     _next_client_id;

    protected:
        mutable mutex_type _mutex; ///< main mutex
        condvar_type       _container_change_condition; ///< main condition variable

    private:
        container_type     _wq_item_container;
        bool               _container_change_flag;
        bool               _worker_stopped_flag;
        thread_type        _worker;
    };
} /* namespace mqmx */
