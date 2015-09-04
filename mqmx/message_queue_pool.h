#ifndef MQMX_MESSAGE_QUEUE_POOL_H_INCLUDED
#define MQMX_MESSAGE_QUEUE_POOL_H_INCLUDED 1

#include <mqmx/message_queue.h>
#include <condition_variable>
#include <vector>

namespace mqmx
{
    class message_queue_pool
    {
        message_queue_pool (const message_queue_pool &) = delete;
        message_queue_pool & operator = (const message_queue_pool &) = delete;

    public:
        class message_handler
        {
        public:
            int operator () (message_queue::message_ptr_type && msg) noexcept
            {
                return handle (std::move (msg));
            }

            virtual int handle (message_queue::message_ptr_type &&) noexcept = 0;

            virtual ~message_handler ()
            {
            }
        };

        typedef std::mutex                       mutex_type;
        typedef std::unique_lock<mutex_type>     lock_type;
        typedef std::condition_variable          condvar_type;
        typedef std::unique_ptr<message_handler> handler_ptr_type;
        typedef std::vector<message_queue>       container_type;
        typedef std::vector<size_t>              counter_container_type;
        typedef std::vector<handler_ptr_type>    handler_container_type;

    public:
        message_queue_pool ();
        ~message_queue_pool ();

        int push (message_queue::message_ptr_type &&);
        int add_queue (const size_t qid, handler_ptr_type &&);

        int wait ();
        int wait_for (const std::chrono::high_resolution_clock::duration &);
        int wait_until (const std::chrono::high_resolution_clock::time_point &);

        void terminate ();
        void dispatch ();

    private:
        mutex_type             _mutex;
        condvar_type           _condition;
        bool                   _has_messages;
        bool                   _terminated;
        container_type         _queue;
        counter_container_type _counter;
        handler_container_type _handler;

        mutex_type             _rwmutex;
        size_t                 _nreaders;
        size_t                 _nwriters;
        condvar_type           _rwcondition;

        int _add (const size_t qid, handler_ptr_type &&);
        int _push (message_queue::message_ptr_type &&);
        void _dispatch_unlocked ();
    };
} /* namespace mqmx */
#endif /* MQMX_MESSAGE_QUEUE_POOL_H_INCLUDED */
