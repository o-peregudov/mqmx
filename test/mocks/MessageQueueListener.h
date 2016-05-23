#pragma once

#include <mqmx/message_queue.h>
#include <gmock/gmock.h>

namespace mocks
{
    struct ListenerMock : mqmx::message_queue::listener
    {
	ListenerMock ();
	virtual ~ListenerMock ();

        MOCK_METHOD3 (notify, void (mqmx::queue_id_type,
                                    mqmx::message_queue *,
                                    const mqmx::message_queue::notification_flags_type));
    };
} /* namespace mocks */
