#include "mqmx/work_queue.h"
#include "test/FakeIt/single_header/standalone/fakeit.hpp"
#include "test/helpers/work_queue_for_tests.h"

#undef NDEBUG
#include <cassert>

struct work_interface
{
    virtual void do_something (const mqmx::work_queue::work_id_type) = 0;
    virtual ~work_interface ()
    {
    }
};

int main ()
{
    using namespace mqmx;
    using namespace fakeit;
    using mock_type = Mock<work_interface>;

    mock_type mock;
    mock_type mock2;
    Fake (Method (mock, do_something));
    Fake (Method (mock2, do_something));
    {
        helpers::work_queue_for_tests sut;

        status_code ec = ExitStatus::Success;
        work_queue::work_id_type work_id = work_queue::INVALID_WORK_ID;
        work_queue::client_id_type client_id = sut.get_client_id ();

        std::tie (ec, work_id) = sut.schedule_work (
            client_id,
            std::bind (&work_interface::do_something, &(mock.get ()), std::placeholders::_1),
            sut.get_current_time_point () + std::chrono::milliseconds (10));

        assert (ec == ExitStatus::Success);
        assert (work_id != work_queue::INVALID_WORK_ID);

        ec = sut.update_work (
            work_id,
            client_id,
            std::bind (&work_interface::do_something, &(mock2.get ()), std::placeholders::_1),
            sut.get_current_time_point () + std::chrono::milliseconds (20),
            work_queue::RUN_ONCE);

        assert (ec == ExitStatus::Success);

        sut.forward_time ();

        ec = sut.cancel_work (work_id);
        assert (ec == ExitStatus::NotFound);

        Verify (Method (mock, do_something).Using (work_id))
            .Never ();
        Verify (Method (mock2, do_something).Using (work_id))
            .Once ();
    }
    VerifyNoOtherInvocations (mock);
    VerifyNoOtherInvocations (mock2);

    return 0;
}
