#include "mqmx/message_queue.h"
#include "test/FakeIt/single_header/standalone/fakeit.hpp"

#undef NDEBUG
#include <cassert>

int main (int argc, const char ** argv)
{
    using namespace fakeit;
    const mqmx::queue_id_type defQID = 10;
    const mqmx::message_id_type defMID = 10;

    Mock<mqmx::message_queue::listener> mock;
    Fake (Method (mock, notify));
    {
	mqmx::message_queue queue (defQID);
	mqmx::message::upointer_type msg;

	mqmx::status_code retCode = queue.set_listener (mock.get ());
	assert (mqmx::ExitStatus::Success == retCode);

	/* test for double insert */
	retCode = queue.set_listener (mock.get ());
	assert (mqmx::ExitStatus::AlreadyExist == retCode);

	retCode = queue.enqueue<mqmx::message> (defMID);
	assert (mqmx::ExitStatus::Success == retCode);

	Verify (Method (mock, notify).Using (
		    defQID, &queue, mqmx::message_queue::notification_flag::data))
	    .Once ();
    }
    Verify (Method (mock, notify).Using (
		defQID, nullptr, mqmx::message_queue::notification_flag::closed))
	.Once ();
    VerifyNoOtherInvocations (mock);
    return 0;
}
