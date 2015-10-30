#include "mqmx/message_queue_poll.h"
#include <cassert>

int main (int argc, const char ** argv)
{
    using namespace mqmx;
    message_queue_poll poll;
    status_code retCode = ExitStatus::Success;
    assert ((retCode == ExitStatus::Success));
    return 0;
}
