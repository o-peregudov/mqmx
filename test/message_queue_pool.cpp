#include "mqmx/message_queue_pool.h"
#include <cassert>

int main (int argc, const char ** argv)
{
    using namespace mqmx;
    message_queue_pool pool;
    status_code retCode = ExitStatus::Success;
    assert ((retCode == ExitStatus::Success));
    return 0;
}
