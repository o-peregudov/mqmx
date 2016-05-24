#pragma once

#include <mqmx/message_queue_poll.h>

namespace fixtures
{
    struct message_queue_poll_fixture
    {
        static const size_t NQUEUES = 10;
        std::vector<mqmx::message_queue::upointer_type> mq;

        message_queue_poll_fixture ();
    };
} /* namespace fixtures */