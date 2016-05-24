#pragma once

#include <mqmx/message_queue.h>
#include <gmock/gmock.h>

namespace mocks
{
    struct message_queue_listener_mock : mqmx::message_queue::listener
    {
	message_queue_listener_mock ();
	virtual ~message_queue_listener_mock ();

        MOCK_METHOD3 (notify, void (mqmx::queue_id_type,
                                    mqmx::message_queue *,
                                    const mqmx::message_queue::notification_flags_type));
    };
} /* namespace mocks */
