#include <mqmx/message_queue.h>
#include <cassert>

namespace mqmx
{
    message::message (const size_t queue_id, const size_t message_id)
	: _qid (queue_id)
	, _mid (message_id)
    {
    }

    message_queue::message_queue (const size_t ID) noexcept
	: _id (ID)
	, _mutex ()
	, _queue ()
    {
    }

    message_queue::message_queue (message_queue && o) noexcept
	: _id (message::undefined_qid)
	, _mutex ()
	, _queue ()
    {
	try
	{
	    lock_type guard (o._mutex);
	    std::swap (_queue, o._queue);
	    std::swap (_id, o._id);
	}
	catch (...)
	{
	}
    }

    message_queue & message_queue::operator = (message_queue && o) noexcept
    {
	try
	{
	    std::lock (_mutex, o._mutex);
	    lock_type guard (_mutex, std::adopt_lock_t ());
	    lock_type oguard (o._mutex, std::adopt_lock_t ());
	    _queue.clear ();
	    _id = message::undefined_qid;
	    std::swap (_queue, o._queue);
	    std::swap (_id, o._id);
	}
	catch (...)
	{
	}
	return *this;
    }

    mqmx_status_code message_queue::push (message_ptr_type && msg)
    {
	if (msg.get () == nullptr)
	{
	    return ExitStatus::InvalidArgument;
	}

	lock_type guard (_mutex);
	if ((_id == message::undefined_qid) ||
	    (_id != msg->get_qid ()))
	{
	    return ExitStatus::NotSupported;
	}

	_queue.push_back (std::move (msg));
	return ExitStatus::Success;
    }

    message_queue::message_ptr_type message_queue::pop ()
    {
	message_ptr_type msg;
	lock_type guard (_mutex);
	if ((_id != message::undefined_qid) &&
	    (_queue.empty () == false))
	{
	    msg = std::move (_queue.front ());
	    _queue.pop_front ();
	}
	return std::move (msg);
    }
} /* namespace mqmx */
