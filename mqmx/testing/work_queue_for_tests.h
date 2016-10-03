#pragma once

#include <mqmx/libexport.h>
#include <mqmx/work_queue.h>
#include <crs/semaphore.h>

namespace mqmx
{
namespace testing
{
    /**
     * \brief Work queue with manual control over internal worker.
     *
     * Original class work_queue relys on wall clock, i.e.
     * triggering of execution of works depends on the system.
     * This is not the best option in tests, when greater control
     * is needed for better synchornization between different parts
     * of the system under test.
     *
     * This class keeps current time in some internal variable and
     * modifies it by the means of special member functions. Each
     * setting of new current time triggers execution of works,
     * which are scheduled not later than new current time.
     *
     * In addition, user callback function can be called
     * (if provided) every time some work has been executed.
     */
    class MQMX_EXPORT work_queue_for_tests : public work_queue
    {
    public:
        /**
         * \brief Type for user synchronization callback.
         *
         * Synchronization callback (if provided) is called every
         * time some work is executed.
         */
        typedef std::function<void (
            const work_queue::work_id_type,
            const work_queue::time_point_type)> sync_function_type;

    private:
        work_queue::time_point_type _current_time_point;

        bool _time_forwarding_flag;
        crs::semaphore _time_forwarding_completed;
        work_queue::client_id_type _time_forwarding_client_id;

        sync_function_type _sync_function;

        virtual bool
        wait_for_time_point (work_queue::lock_type &,
                             const work_queue::time_point_type &) override;

        virtual work_queue::time_point_type
        execute_work (work_queue::lock_type &,
                      const work_queue::record_type &) override;

        bool wait_for_time_forwarding_completion (
            const work_queue::time_point_type final_time_point);

    public:
        /**
         * \brief Default constructor.
         */
        work_queue_for_tests (const sync_function_type & = sync_function_type ());

        /**
         * \brief Virtual destructor.
         */
        virtual ~work_queue_for_tests ();

        /**
         * \brief Returns current time.
         */
        virtual work_queue::time_point_type get_current_time_point () const override;

        /**
         * \brief Set new current time to the timestamp of the
         *        earliest work waiting for execution.
         *
         * Changing current time will trigger execution for each
         * work scheduled not later than this new current time.
         *
         * In this case only earliest works scheduled for the
         * same time will be triggered.
         *
         * \param wait_for_completion should be set to 'true' in
         *        case this call should block waiting for all works
         *        (and synchronization callback function) are to be
         *        executed
         *
         * \returns true in case of success, or false otherwise
         */
        bool forward_time (const bool wait_for_completion = true);

        /**
         * \brief Set new current time.
         *
         * Changing current time will trigger execution for each
         * work scheduled not later than this new current time.
         *
         * \param new_time is the new value for current time
         * \param wait_for_completion should be set to 'true' in
         *        case this call should block waiting for all works
         *        (and synchronization callback function) are to be
         *        executed
         *
         * \returns true in case of success, or false otherwise
         */
        bool forward_time (const work_queue::time_point_type & new_time,
                           const bool wait_for_completion = true);

        /**
         * \brief Change current time.
         *
         * Changing current time will trigger execution for each
         * work scheduled not later than this new current time.
         *
         * This version allows to add some duration to the current
         * time.
         *
         * \param rel_time is the new offset, that should be added
         *        to the current time
         * \param wait_for_completion should be set to 'true' in
         *        case this call should block waiting for all works
         *        (and synchronization callback function) are to be
         *        executed
         *
         * \returns true in case of success, or false otherwise
         */
        bool forward_time (const work_queue::duration_type & rel_time,
                           const bool wait_for_completion = true);
    };
} /* namespace testing */
} /* namespace mqmx */
