#pragma once

#include <mqmx/libexport.h>
#include <mqmx/work_queue.h>
#include <crs/semaphore.h>

namespace mqmx
{
namespace testing
{
    class MQMX_EXPORT work_queue_for_tests : public work_queue
    {
    public:
        typedef std::function<void (
            const work_queue::work_id_type,
            const work_queue::time_point_type)> sync_function_type;

    private:
        work_queue::time_point_type _current_time_point;

        bool _time_forwarding_flag;
        crs::semaphore _time_forwarding_completed;
        work_queue::client_id_type _time_forwarding_client_id;

        sync_function_type _sync_function;

        virtual void
        signal_going_to_idle (work_queue::lock_type &) override;

        virtual bool
        wait_for_time_point (work_queue::lock_type &,
                             const work_queue::time_point_type &) override;

        virtual work_queue::time_point_type
        execute_work (work_queue::lock_type &,
                      const work_queue::record_type &) override;

        bool wait_for_time_forwarding_completion (
            const work_queue::time_point_type final_time_point);

    public:
        work_queue_for_tests (const sync_function_type & = sync_function_type ());
        virtual ~work_queue_for_tests ();

        virtual work_queue::time_point_type get_current_time_point () const override;

        bool forward_time (const bool wait_for_completion = true);

        bool forward_time (const work_queue::time_point_type &,
                           const bool wait_for_completion = true);

        bool forward_time (const work_queue::duration_type &,
                           const bool wait_for_completion = true);
    };
} /* namespace testing */
} /* namespace mqmx */
