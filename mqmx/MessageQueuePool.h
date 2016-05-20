#pragma once

#include <Common/OAMThreading.hpp>
#include <mqmx/MessageQueuePoll.h>

#include <crs/mutex.h>
#include <crs/condition_variable.h>
#include <crs/semaphore.h>

#include <functional>
#include <vector>

namespace mqmx
{
    class MessageQueuePool
    {
    public:
        struct mq_deleter
        {
            MessageQueuePool * _pool;

            mq_deleter (MessageQueuePool * pool = nullptr)
                : _pool (pool)
            { }

            mq_deleter (const mq_deleter & o) = default;
            mq_deleter & operator = (const mq_deleter & o) = default;

            mq_deleter (mq_deleter && o) = default;
            mq_deleter & operator = (mq_deleter && o) = default;

            void operator () (mqmx::MessageQueue * mq) const
            {
                if (_pool)
                    _pool->removeQueue (mq);
            }
        };

        friend class mq_deleter;

        typedef std::function<status_code(Message::upointer_type &&)> message_handler_func_type;
        typedef CrossClass::mutex_type                                mutex_type;
        typedef CrossClass::lock_type                                 lock_type;
        typedef CrossClass::condvar_type                              condvar_type;
        typedef CrossClass::semaphore                                 semaphore_type;
        typedef std::unique_ptr<MessageQueue, mq_deleter>             mq_upointer_type;

    private:
        typedef std::vector<message_handler_func_type>                handlers_map_type;

        struct add_queue_message;
        friend struct add_queue_message;

        struct remove_queue_message;
        friend struct remove_queue_message;

        static const queue_id_type   CONTROL_MESSAGE_QUEUE_ID;
        static const message_id_type TERMINATE_MESSAGE_ID;
        static const message_id_type POLL_PAUSE_MESSAGE_ID;
        static const message_id_type ADD_QUEUE_MESSAGE_ID;
        static const message_id_type REMOVE_QUEUE_MESSAGE_ID;

        MessageQueue                m_mqControl;
        handlers_map_type           m_mqHandler;
        std::vector<MessageQueue *> m_mqs;
        semaphore_type              m_pauseSemaphore;
        semaphore_type              m_resumeSemaphore;
        BBC_pkg::oam_thread_type    m_auxThread;

        status_code removeQueue (const MessageQueue * const);
        status_code controlQueueHandler (Message::upointer_type &&);
        status_code handleNotifications (const MessageQueuePoll::notification_rec_type &);
        void threadLoop ();

    public:
        explicit MessageQueuePool (const size_t capacity = 10);
        ~MessageQueuePool ();

        bool isPollIdle ();

        mq_upointer_type allocateQueue (const message_handler_func_type &);
    };
} /* namespace mqmx */
