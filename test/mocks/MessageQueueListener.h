#pragma once

#include <mqmx/MessageQueue.h>
#include <gmock/gmock.h>

namespace mocks
{
    struct ListenerMock : mqmx::MessageQueue::Listener
    {
        MOCK_METHOD3 (notify, void (mqmx::queue_id_type,
                                    mqmx::MessageQueue *,
                                    const mqmx::MessageQueue::notification_flags_type));
    };
} /* namespace mocks */
