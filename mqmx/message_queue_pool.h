#pragma once

#include <mqmx/message_queue_poll.h>

#include <crs/mutex.h>
#include <crs/condition_variable.h>
#include <crs/semaphore.h>

#include <functional>
#include <vector>
#include <thread>

namespace mqmx
{
    class message_queue_pool
    {
    public:
        struct mq_deleter
        {
            message_queue_pool * _pool;

            mq_deleter (message_queue_pool * pool = nullptr)
                : _pool (pool)
            { }

            mq_deleter (const mq_deleter & o) = default;
            mq_deleter & operator = (const mq_deleter & o) = default;

            mq_deleter (mq_deleter && o) = default;
            mq_deleter & operator = (mq_deleter && o) = default;

            void operator () (mqmx::message_queue * mq) const
            {
                if (_pool)
                    _pool->remove_queue (mq);
            }
        };

        friend class mq_deleter;

        typedef std::function<status_code(message::upointer_type &&)> message_handler_func_type;
        typedef CrossClass::mutex_type                                mutex_type;
        typedef CrossClass::lock_type                                 lock_type;
        typedef CrossClass::condvar_type                              condvar_type;
        typedef CrossClass::semaphore                                 semaphore_type;
        typedef std::unique_ptr<message_queue, mq_deleter>            mq_upointer_type;

    private:
        typedef std::vector<message_handler_func_type>                handlers_map_type;
        typedef std::thread                                           thread_type;

        struct add_queue_message;
        friend struct add_queue_message;

        struct remove_queue_message;
        friend struct remove_queue_message;

        static const queue_id_type   CONTROL_MESSAGE_QUEUE_ID;
        static const message_id_type TERMINATE_MESSAGE_ID;
        static const message_id_type POLL_PAUSE_MESSAGE_ID;
        static const message_id_type ADD_QUEUE_MESSAGE_ID;
        static const message_id_type REMOVE_QUEUE_MESSAGE_ID;

        message_queue                _mq_control;
        handlers_map_type            _handler;
        std::vector<message_queue *> _mqs;
        semaphore_type               _sem_pause;
        semaphore_type               _sem_resume;
        thread_type                  _worker;

        status_code remove_queue (const message_queue * const);
        status_code control_queue_handler (message::upointer_type &&);
        status_code handle_notifications (const message_queue_poll::notification_rec_type &);
        void thread_loop ();

    public:
        explicit message_queue_pool (const size_t capacity = 10);
        ~message_queue_pool ();

        bool is_poll_idle ();

        mq_upointer_type allocate_queue (const message_handler_func_type &);
    };
} /* namespace mqmx */
