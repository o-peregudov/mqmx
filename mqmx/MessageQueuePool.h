#pragma once

#include <Common/OAMThreading.hpp>
#include <mqmx/MessageQueuePoll.h>

#include <atomic>
#include <functional>
#include <map>

namespace mqmx
{
    class MessageQueuePool
    {
    public:
	typedef std::function<status_code(Message::upointer_type &&)> message_handler_func_type;
	typedef std::mutex                                            mutex_type;
	typedef std::unique_lock<mutex_type>                          lock_type;
	typedef BBC_pkg::oam_condvar_type                             condvar_type;

    private:
	typedef std::map<queue_id_type, message_handler_func_type>    handlers_map_type;

	const queue_id_type   CONTROL_MESSAGE_QUEUE_ID = 0x00;
	const message_id_type TERMINATE_MESSAGE_ID = 0x00;
	const message_id_type POLL_RESTART_MESSAGE_ID = 0x01;

	MessageQueue                m_mqControl;
        handlers_map_type           m_mqHandler;
        std::vector<MessageQueue *> m_mqs;
        bool                        m_terminateFlag;
        std::atomic<bool>           m_restartFlag;
        mutable mutex_type          m_pollMutex;
        condvar_type                m_pollCondition;
        BBC_pkg::oam_thread_type    m_auxThread;

	status_code controlQueueHandler (Message::upointer_type &&);
        bool handleNotifications (const size_t nQueuesSignaled, /* total number of mqs signaled */
                                  const size_t idxCurrentQueue, /* current number of signaled mq */
                                  const MessageQueuePoll::notification_rec_type &);
	void threadLoop ();

    public:
	MessageQueuePool ();
	~MessageQueuePool ();

	MessageQueue::upointer_type addQueue (const message_handler_func_type &);
	status_code removeQueue (const MessageQueue * const);
    };
} /* namespace mqmx */
