#ifndef MQMX_MESSAGE_QUEUE_H_INCLUDED
#define MQMX_MESSAGE_QUEUE_H_INCLUDED 1

#include <memory>
#include <mutex>
#include <deque>

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

    typedef int    mqmx_status_code;
    typedef size_t mqmx_queue_id_type;
    typedef size_t mqmx_message_id_type;

    class message
    {
        const mqmx_queue_id_type _qid;
        const mqmx_message_id_type _mid;

    public:
        static const mqmx_queue_id_type undefined_qid =
	    static_cast<mqmx_queue_id_type> (-1);

        message (const mqmx_queue_id_type queue_id,
		 const mqmx_message_id_type message_id);
        virtual ~message ()
	{
	}

        mqmx_queue_id_type get_qid () const
        {
            return _qid;
        }

        mqmx_message_id_type get_mid () const
        {
            return _mid;
        }
    };

    class message_queue
    {
        message_queue (const message_queue &) = delete;
        message_queue & operator = (const message_queue &) = delete;

    public:
        typedef std::unique_ptr<message>     message_ptr_type;
        typedef std::mutex                   mutex_type;
        typedef std::unique_lock<mutex_type> lock_type;
        typedef std::deque<message_ptr_type> container_type;

    public:
        message_queue (const mqmx_queue_id_type = message::undefined_qid) noexcept;
        message_queue (message_queue && o) noexcept;

        message_queue & operator = (message_queue && o) noexcept;

        mqmx_status_code push (message_ptr_type && msg);
        message_ptr_type pop ();

    private:
        mqmx_queue_id_type _id;
        mutex_type         _mutex;
        container_type     _queue;
    };
} /* namespace mqmx */
#endif /* MQMX_MESSAGE_QUEUE_H_INCLUDED */
