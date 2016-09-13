#pragma once

#include <mqmx/libexport.h>
#include <mqmx/work_queue.h>

namespace mqmx
{
namespace testing
{
    class MQMX_EXPORT work_queue_for_tests : public mqmx::work_queue
    {
    public:
        typedef std::function<void (
	    const mqmx::work_queue::work_id_type,
	    const mqmx::work_queue::time_point_type)> sync_function_type;

    private:
        mqmx::work_queue::time_point_type _current_time_point;
        bool _time_point_changed;

        mqmx::work_queue::condvar_type _time_forwarding_condition;
        bool _time_forwarding_completed;

        sync_function_type _sync_function;

        virtual void
        signal_going_to_idle (mqmx::work_queue::lock_type &) override;

        virtual bool
        wait_for_time_point (mqmx::work_queue::lock_type &,
			     const mqmx::work_queue::time_point_type &) override;

        virtual mqmx::work_queue::time_point_type
        execute_work (mqmx::work_queue::lock_type &,
		      const mqmx::work_queue::record_type &) override;

    public:
        work_queue_for_tests (const sync_function_type & = sync_function_type ());
        virtual ~work_queue_for_tests ();

        virtual mqmx::work_queue::time_point_type get_current_time_point () const override;

        void forward_time (const bool wait_for_completion = true);

        mqmx::work_queue::time_point_type
        forward_time (const mqmx::work_queue::time_point_type &,
		      const bool wait_for_completion = true);

        mqmx::work_queue::time_point_type
        forward_time (const mqmx::work_queue::duration_type &,
		      const bool wait_for_completion = true);
    };
} /* namespace testing */
} /* namespace mqmx */
