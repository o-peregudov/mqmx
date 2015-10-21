#include "mqmx/message_queue_pool.h"
#include <cassert>

int main (int argc, const char ** argv)
{
    using namespace mqmx;
    message_queue_pool pool;
    int retCode = ExitStatus::Success;
    while (retCode != ExitStatus::Finished)
    {
        retCode = pool.wait_for (std::chrono::milliseconds (100));
        if (retCode == ExitStatus::Success)
        {
            pool.dispatch ();
        }
        else if (retCode == ExitStatus::Timeout)
        {
            break;
        }
    }
    return 0;
}
