#include "mqmx/work_queue.h"
#include "test/FakeIt/single_header/standalone/fakeit.hpp"
#include <crs/semaphore.h>

#undef NDEBUG
#include <cassert>

struct work_interface
{
    virtual void do_something (const mqmx::work_queue::work_id_type) = 0;
    virtual ~work_interface () { }
};

int main ()
{
    using namespace mqmx;
    using namespace fakeit;
    using mock_type = Mock<work_interface>;

    const size_t nrepetitions = 5;
    crs::semaphore sem;

    mock_type mock;
    When (Method (mock, do_something)).AlwaysDo (
        [&sem, nrepetitions](const work_queue::work_id_type)
        {
            static size_t ninvocations = nrepetitions;
            if (--ninvocations == 0)
            {
                sem.post ();
                throw int (0);
            }
        });
    {
        work_queue sut;

        status_code ec = ExitStatus::Success;
        work_queue::work_id_type work_id = work_queue::INVALID_WORK_ID;
        work_queue::client_id_type client_id = sut.get_client_id ();

        const auto execution_period = std::chrono::milliseconds (1);
        std::tie (ec, work_id) = sut.schedule_work (
            client_id,
            std::bind (&work_interface::do_something,
                       &(mock.get ()),
                       std::placeholders::_1),
            sut.get_current_time_point (),
            execution_period);

        assert (ec == ExitStatus::Success);
        assert (work_id != work_queue::INVALID_WORK_ID);

        assert (sem.wait_for (std::chrono::seconds (1)));
        std::this_thread::yield ();

        Verify (Method (mock, do_something).Using (work_id))
            .Exactly (nrepetitions);
    }
    VerifyNoOtherInvocations (mock);

    return 0;
}
