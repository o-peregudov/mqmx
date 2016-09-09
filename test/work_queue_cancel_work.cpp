#include "mqmx/work_queue.h"
#include "test/FakeIt/single_header/standalone/fakeit.hpp"

#undef NDEBUG
#include <cassert>

struct work_interface
{
    virtual bool do_something (const mqmx::work_queue::work_id_type) = 0;
    virtual ~work_interface () { }
};

int main ()
{
    using namespace mqmx;
    using namespace fakeit;
    using mock_type = Mock<work_interface>;

    mock_type mock;
    When (Method (mock, do_something)).AlwaysReturn (true);
    {
        work_queue sut;

        status_code ec = ExitStatus::Success;
        work_queue::work_id_type work_id = work_queue::INVALID_WORK_ID;
        work_queue::client_id_type client_id = sut.get_client_id ();

        std::tie (ec, work_id) = sut.schedule_work (
            client_id,
            std::bind (&work_interface::do_something, &(mock.get ()), std::placeholders::_1),
            sut.get_current_time_point () + std::chrono::hours (1));

        assert (ec == ExitStatus::Success);
        assert (work_id != work_queue::INVALID_WORK_ID);

        ec = sut.cancel_work (work_id);
        assert (ec == ExitStatus::Success);

        ec = sut.cancel_work (work_id);
        assert (ec == ExitStatus::NotFound);

        Verify (Method (mock, do_something).Using (work_id))
            .Never ();
    }
    VerifyNoOtherInvocations (mock);

    return 0;
}
