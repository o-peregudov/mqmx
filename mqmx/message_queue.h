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

    typedef int mqmx_status_code;
    
    class message
    {
        const size_t _qid;
        const size_t _mid;

    public:
        static const size_t undefined_qid = static_cast<size_t> (-1);

        message (const size_t queue_id, const size_t message_id);
        virtual ~message ()
	{
	}

        size_t get_qid () const
        {
            return _qid;
        }

        size_t get_mid () const
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
        message_queue (const size_t ID = message::undefined_qid) noexcept;
        message_queue (message_queue && o) noexcept;

        message_queue & operator = (message_queue && o) noexcept;

        mqmx_status_code push (message_ptr_type && msg);
        message_ptr_type pop ();

    private:
        size_t         _id;
        mutex_type     _mutex;
        container_type _queue;
    };
} /* namespace mqmx */
#endif /* MQMX_MESSAGE_QUEUE_H_INCLUDED */
