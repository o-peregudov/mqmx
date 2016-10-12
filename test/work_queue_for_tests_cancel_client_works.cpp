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

int main ()
{
    using namespace mqmx;
    using namespace fakeit;
    using mock_type = Mock<work_interface>;

    mock_type mockA;
    mock_type mockB;
    When (Method (mockA, do_something)).AlwaysReturn (true);
    When (Method (mockB, do_something)).AlwaysReturn (true);
    {
        testing::work_queue_for_tests sut;

        status_code ec = ExitStatus::Success;
        work_queue::work_id_type workA_id = work_queue::INVALID_WORK_ID;
        work_queue::work_id_type workB_id = work_queue::INVALID_WORK_ID;
        work_queue::client_id_type clientA_id = sut.get_client_id ();
        work_queue::client_id_type clientB_id = sut.get_client_id ();

        const auto execution_period = std::chrono::milliseconds (10);
        std::tie (ec, workA_id) = sut.schedule_work (
            clientA_id,
            std::bind (&work_interface::do_something,
                       &(mockA.get ()),
                       std::placeholders::_1),
            sut.get_current_time_point (),
            execution_period);

        assert (ec == ExitStatus::Success);
        assert (workA_id != work_queue::INVALID_WORK_ID);

        std::tie (ec, workB_id) = sut.schedule_work (
            clientB_id,
            std::bind (&work_interface::do_something,
                       &(mockB.get ()),
                       std::placeholders::_1),
            sut.get_current_time_point (),
            execution_period);

        assert (ec == ExitStatus::Success);
        assert (workB_id != work_queue::INVALID_WORK_ID);

        sut.forward_time (execution_period / 2);

        ec = sut.cancel_client_works (clientA_id);
        assert (ec == ExitStatus::Success);

        const size_t nrepetitions = 3;
        sut.forward_time (nrepetitions * execution_period);

        Verify (Method (mockA, do_something).Using (workA_id))
            .Once ();
        Verify (Method (mockB, do_something).Using (workB_id))
            .Exactly (nrepetitions + 1);
    }
    VerifyNoOtherInvocations (mockA);
    VerifyNoOtherInvocations (mockB);

    return 0;
}
