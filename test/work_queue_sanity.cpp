#include "mqmx/work_queue.h"

#undef NDEBUG
#include <cassert>

int main ()
{
    using namespace mqmx;

    work_queue sut;
    assert (sut.is_idle ());
    assert (sut.get_nearest_time_point ().time_since_epoch ().count () == 0);

    auto dummy_work = [](const work_queue::work_id_type){
        return true;
    };

    status_code ec = ExitStatus::Success;
    work_queue::work_id_type work_id = work_queue::INVALID_WORK_ID;
    work_queue::client_id_type client_id = sut.get_client_id ();
    assert (client_id != work_queue::INVALID_CLIENT_ID);

    std::tie (ec, work_id) = sut.schedule_work (work_queue::INVALID_CLIENT_ID, dummy_work);
    assert (ec == ExitStatus::InvalidArgument);

    std::tie (ec, work_id) = sut.schedule_work (client_id, work_queue::work_pointer_type ());
    assert (ec == ExitStatus::InvalidArgument);

    ec = sut.update_work (work_queue::INVALID_WORK_ID,
                          client_id,
                          dummy_work,
                          std::chrono::steady_clock::now (),
                          std::chrono::seconds (0));
    assert (ec == ExitStatus::NotFound);

    ec = sut.update_work (work_id,
                          work_queue::INVALID_CLIENT_ID,
                          dummy_work,
                          std::chrono::steady_clock::now (),
                          std::chrono::seconds (0));
    assert (ec == ExitStatus::InvalidArgument);

    ec = sut.update_work (work_id,
                          client_id,
                          work_queue::work_pointer_type (),
                          std::chrono::steady_clock::now (),
                          std::chrono::seconds (0));
    assert (ec == ExitStatus::InvalidArgument);

    ec = sut.cancel_work (work_queue::INVALID_WORK_ID);
    assert (ec == ExitStatus::NotFound);

    ec = sut.cancel_client_works (client_id);
    assert (ec == ExitStatus::NotFound);

    sut.kill_worker ();

    std::tie (ec, work_id) = sut.schedule_work (client_id, dummy_work);
    assert (ec == ExitStatus::NotAllowed);

    ec = sut.update_work (work_queue::INVALID_WORK_ID,
                          client_id,
                          dummy_work,
                          std::chrono::steady_clock::now (),
                          std::chrono::seconds (0));
    assert (ec == ExitStatus::NotAllowed);

    ec = sut.cancel_work (work_queue::INVALID_WORK_ID);
    assert (ec == ExitStatus::NotAllowed);

    ec = sut.cancel_client_works (client_id);
    assert (ec == ExitStatus::NotAllowed);

    return 0;
}
