#include "mqmx/testing/work_queue_for_tests.h"
#include "test/FakeIt/single_header/standalone/fakeit.hpp"

#undef NDEBUG
#include <cassert>

struct work_interface
{
    virtual bool do_something (const mqmx::work_queue::work_id_type) = 0;
    virtual ~work_interface ()
    {
    }
};

struct sync_interface
{
    virtual void do_something (
        const mqmx::work_queue::work_id_type,
        const mqmx::work_queue::time_point_type) = 0;
    virtual ~sync_interface ()
    {
    }
};

int main ()
{
    using namespace mqmx;
    using namespace fakeit;
    using work_mock_type = Mock<work_interface>;
    using sync_mock_type = Mock<sync_interface>;

    work_mock_type work_mock;
    sync_mock_type sync_mock;
    When (Method (work_mock, do_something)).AlwaysReturn (true);
    Fake (Method (sync_mock, do_something));
    {
        testing::work_queue_for_tests sut (
            std::bind (&sync_interface::do_something,
                       &(sync_mock.get ()),
                       std::placeholders::_1,
                       std::placeholders::_2));

        status_code ec = ExitStatus::Success;
        work_queue::work_id_type work_id = work_queue::INVALID_WORK_ID;
        work_queue::client_id_type client_id = sut.get_client_id ();

        const auto execution_period = std::chrono::milliseconds (10);
        std::tie (ec, work_id) = sut.schedule_work (
            client_id,
            std::bind (&work_interface::do_something,
                       &(work_mock.get ()),
                       std::placeholders::_1),
            sut.get_current_time_point () + execution_period / 2,
            execution_period);

        assert (ec == ExitStatus::Success);
        assert (work_id != work_queue::INVALID_WORK_ID);

        const size_t nrepetitions = 5;
        sut.forward_time (nrepetitions * execution_period);

        Verify (Method (work_mock, do_something).Using (work_id) +
                Method (sync_mock, do_something).Using (work_id, _))
            .Exactly (nrepetitions);
    }
    VerifyNoOtherInvocations (work_mock);
    VerifyNoOtherInvocations (sync_mock);

    return 0;
}
