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
     * This is generic work queue (WQ).
     *
     * Main idea behind this implementation is to execute user work
     * at specified moment of time (asynchronously).
     *
     * Class is non-copyable but movable (yet move constructor/operator
     * are not implemented).
     *
     * \note Because of implementation details very short timeout values
     *       (usually less than 10 ms) cannot be guaranteed.
     *       Actual precision was not estimated.
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

        static const client_id_type INVALID_CLIENT_ID;
        static const work_id_type   INVALID_WORK_ID;
	static const duration_type  RUN_ONCE;

    protected:
        /**
         * Basic data structure used by internal WQ implementation
         *
         * Contains the following fields:
         *  - time_point (when the job should be executed)
         *  - pointer to a work function
         *  - duration of periodic invocation (if this functionality
         *    is required, if not - work will be executed only once)
         *  - ID of the work's owner
         */
        struct wq_item
        {
            time_point_type   time_point;
            client_id_type    client_id;
            work_pointer_type work;
            duration_type     period;

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
         * Custom comparator used by internal WQ implementation
         *
         * STL priority queue requires invert implementation and
         * this class guarantees the necessary ordering.
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
         * Default constructor
         *
         * Initializes all internal data members and starts
         * internal worker thread.
         */
        work_queue ();

        /**
         * Destructor
         *
         * Terminates internal worker thread and clears all
         * internal data members.
         *
         * \remark Despite the fact internal worker thread is terminated
         *         automatically it is always a good idea to stop it
         *         explicitly before destruction of instance of WQ
         */
        virtual ~work_queue ();

        /**
         * Checks, whether worker thread is waiting for any work to be
         * scheduled (i.e. internal queue is empty) or not.
         */
        bool is_idle () const;

        /**
         * Terminates worker thread immediately, but gently
         *
         * All works, which were present in queue, but were not
         * processed will be discarded.
         *
         * \note
         *  - you cannot put new works into queue after this call;
         *  - this is kind of final call, because there are no possibility to
         *    restart (recreate) internal worker thread.
         */
        void kill_worker ();

        /**
         * Schedule new work for asynchronous execution
         *
         * \param work is a poiter to function, that should be executed
         * \param client_id is an ID, aimed to group work items which belongs to
         *        some particular client
         * \param start_time is a time point, when work execution
         *        should be triggered for the first time
         * \param repeat_timeout is a timeout, that is used to periodically
         *        re-trigger work execution since first trigger (it can be set
         *        to 0 if periodic triggering is not needed)
         *
         * \returns <i>ExitStatus::InvalidArgument</i> if work pointer
         *          is null
         * \returns <i>ExitStatus::NotAllowed</i> if worker thread
         *          is terminated
         * \returns <i>ExitStatus::Success</i> in case of success
         */
        std::pair<status_code, work_id_type> schedule_work (
            const client_id_type client_id,
            const work_pointer_type & work,
            const time_point_type & start_time = time_point_type (),
            const duration_type & repeat_timeout = RUN_ONCE);

        /**
         * Update work with given ID.
         *
         * With this call you can change work item completely -- existing
         * record will be replaced with new one, only ID of event will be
         * preserved.
         *
         * \param work_id is the ID of already posted work to be altered
         * \param work is a poiter to function, that should be executed
         * \param client_id is an ID, aimed to group work items which belongs to
         *        some particular client
         * \param start_time is a time point, when work should be triggered
         *        for the first time
         * \param repeat_timeout is a timeout, that is used to periodically
         *        re-trigger work execution since first trigger (it can be set
         *        to 0 if periodic triggering is not needed)
         *
         * \returns <i>ExitStatus::InvalidArgument</i> if work pointer
         *          is null
         * \returns <i>ExitStatus::NotAllowed</i> if worker thread
         *          is terminated
         * \returns <i>ExitStatus::NotFound</i> if work with given ID
         *          is not in the queue
         * \returns <i>ExitStatus::Success</i> in case of success
         */
        status_code update_work (const work_id_type work_id,
                                 const client_id_type client_id,
                                 const work_pointer_type & work,
                                 const time_point_type & start_time,
                                 const duration_type & repeat_timeout);

        /**
         * Cancel timer event with given ID
         *
         * \param work_id is the ID of already posted work to be canceled
         *
         * \returns <i>ExitStatus::NotAllowed</i> if worker thread
         *          is terminated
         * \returns <i>ExitStatus::NotFound</i> if work with given ID
         *          is not in the queue
         * \returns <i>ExitStatus::Success</i> in case of success
         */
        status_code cancel_work (const work_id_type work_id);

        /**
         * Cancel all work items, which belongs to specified owner
         *
         * \returns <i>ExitStatus::NotAllowed</i> if worker thread
         *          is terminated
         * \returns <i>ExitStatus::NotFound</i> if no events were listed
         *          in parameter
         * \returns <i>ExitStatus::Success</i> if events were removed from
         *          the queue
         */
        status_code cancel_client_works (const client_id_type client_id);

        /**
         * Get unique client ID
         *
         * Unique client ID is needed for grouping work item by client/owner.
         */
        client_id_type get_client_id ();

        /**
         * Get nearest time point
         *
         * \returns nearest time point or empty time point (set to epoch)
         *          if there are no events scheduled
         *
         * \note This method is in general needed for tests.
         */
        time_point_type get_nearest_time_point () const;

        /**
         * Get current time point
         *
         * \returns clock_type::now ()
         */
        virtual time_point_type get_current_time_point () const;

    protected:
        bool wait_for_some_work (lock_type &);

        void reset_container_change_flag (lock_type &);
        bool get_container_change_flag (lock_type &) const;

        bool is_container_empty (lock_type &) const;
        bool is_time_point_empty (const time_point_type &) const;

        time_point_type get_empty_time_point () const;

        virtual void signal_going_to_idle (lock_type &);
        virtual bool wait_for_time_point (lock_type &, const time_point_type &);
        virtual time_point_type execute_work (lock_type &, const record_type &);

    private:
        std::pair<status_code, work_id_type> post_work (lock_type &, wq_item);

        void signal_container_change (lock_type &);
        void make_heap_and_notify_worker (lock_type &);
        bool signal_worker_to_stop ();
        void worker ();

        work_id_type       _next_work_id;
        client_id_type     _next_client_id;

    protected:
        mutable mutex_type _mutex;
        condvar_type       _container_change_condition;

    private:
        container_type     _wq_item_container;
        bool               _container_change_flag;
        bool               _worker_stopped_flag;
        thread_type        _worker;
    };
} /* namespace mqmx */
