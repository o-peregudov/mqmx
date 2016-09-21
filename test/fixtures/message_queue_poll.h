#pragma once

#include <mqmx/libexport.h>
#include "mqmx/message_queue_poll.h"

namespace fixtures
{
    struct MQMX_EXPORT message_queue_poll
    {
        static const size_t NQUEUES = 10;
        std::vector<mqmx::message_queue::upointer_type> mq;

        message_queue_poll ();
    };
} /* namespace fixtures */
