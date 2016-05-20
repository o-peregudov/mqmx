#pragma once

#include <cstddef>

namespace mqmx
{
    enum ExitStatus
    {
	Success = 0,
	Finished,
	Timeout,
	AlreadyExist,
	InvalidArgument,
	NotSupported,
	NotFound,
	RestartNeeded,
	HaltRequested,
	PauseRequested,
	NotAllowed,
    };

    typedef int    status_code;
    typedef size_t queue_id_type;
    typedef size_t message_id_type;
} /* namespace mqmx */
