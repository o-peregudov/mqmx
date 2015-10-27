#ifndef MQMX_TYPES_H_INCLUDED
#define MQMX_TYPES_H_INCLUDED 1

#include <cstdarg>

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
    };

    typedef int    status_code;
    typedef size_t queue_id_type;
    typedef size_t message_id_type;
} /* namespace mqmx */
#endif /* MQMX_TYPES_H_INCLUDED */
