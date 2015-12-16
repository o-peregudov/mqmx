#include "mqmx/MessageQueuePool.h"

namespace mqmx
{
    status_code MessageQueuePool::controlQueueHandler (Message::upointer_type && msg)
    {
	if (msg->getMID () == TERMINATE_MESSAGE_ID)
	{
	    m_terminateFlag = true;
	}
	else if (msg->getMID () == POLL_RESTART_MESSAGE_ID)
	{
	    m_restartFlag = true;
	}
	return ExitStatus::Success;
    }

    void MessageQueuePool::handleNotifications (const MessageQueuePoll::notification_rec_type & rec)
    {
	if (std::get<2> (rec) & MessageQueue::NotificationFlag::Detached)
	{
	    if (std::get<0> (rec) == m_mqControl.getQID ())
	    {
		m_terminateFlag = true;
		return;
	    }
	}

	if (std::get<2> (rec) & MessageQueue::NotificationFlag::NewData)
	{
	    Message::upointer_type msg = std::get<1> (rec)->pop ();
	    if (std::get<0> (rec) == m_mqControl.getQID ())
	    {
		controlQueueHandler (std::move (msg));
		return;
	    }

	    const auto it = m_mqHandler.find (std::get<0> (rec));
	    if (it->second)
	    {
		const status_code retCode = (it->second)(std::move (msg));
		if (retCode != ExitStatus::Success)
		{
		    /* print warning about not success */
		}
	    }
	}
    }

    void MessageQueuePool::threadLoop ()
    {
	for (m_mqs.push_back (&m_mqControl); !m_terminateFlag; )
	{
	    MessageQueuePoll mqp;
	    const auto mqlist = mqp.poll (std::begin (m_mqs), std::end (m_mqs),
					  WaitTimeProvider::WAIT_INFINITELY);

	    for (const auto & elem : mqlist)
	    {
		handleNotifications (elem);
	    }

	    if (m_restartFlag)
	    {
		lock_type guard (m_pollMutex);
		m_pollCondition.notify_one ();
		m_pollCondition.wait (guard, [this]{ return !m_restartFlag; });
	    }
	}
    }

    MessageQueuePool::MessageQueuePool ()
	: m_mqControl (CONTROL_MESSAGE_QUEUE_ID)
	, m_mqHandler ()
	, m_mqs ()
	, m_terminateFlag (false)
	, m_restartFlag (false)
	, m_pollMutex ()
	, m_pollCondition ()
	, m_auxThread ([this]{ threadLoop (); })
    {
	const message_handler_func_type handler = std::bind (
	    &MessageQueuePool::controlQueueHandler, this, std::placeholders::_1);
	m_mqHandler.insert (std::make_pair (m_mqControl.getQID (), handler));
    }

    MessageQueuePool::~MessageQueuePool ()
    {
	m_mqControl.enqueue<Message> (TERMINATE_MESSAGE_ID);
	m_auxThread.join ();
    }

    MessageQueue::upointer_type MessageQueuePool::addQueue (const message_handler_func_type & handler)
    {
	MessageQueue::upointer_type newMQ;
	if (handler)
	{
	    lock_type guard (m_pollMutex);
	    m_mqControl.enqueue<Message> (POLL_RESTART_MESSAGE_ID);
	    m_pollCondition.wait (guard, [this]{ return m_restartFlag.load (); });

	    auto it = --(m_mqHandler.end ());
	    newMQ.reset (new MessageQueue (it->first + 1));

	    auto ires = m_mqHandler.insert (std::make_pair (newMQ->getQID (), handler));
	    if (ires.second)
	    {
		m_mqs.push_back (newMQ.get ());
	    }

	    m_restartFlag = false;
	    m_pollCondition.notify_one ();
	}
	return newMQ;
    }

    status_code MessageQueuePool::removeQueue (MessageQueue * mq)
    {
	if (mq == nullptr)
	{
	    return ExitStatus::InvalidArgument;
	}

	lock_type guard (m_pollMutex);
	m_mqControl.enqueue<Message> (POLL_RESTART_MESSAGE_ID);
	m_pollCondition.wait (guard, [this]{ return m_restartFlag.load (); });

	bool found = false;
	for (size_t ix = 0; ix < m_mqs.size (); ++ix)
	{
	    if (m_mqs[ix] == mq)
	    {
		found = true;
		std::swap (m_mqs[ix], m_mqs.back ());
		m_mqs.pop_back ();
		break;
	    }
	}

	if (found)
	{
	    m_mqHandler.erase (mq->getQID ());
	}

	m_restartFlag = false;
	m_pollCondition.notify_one ();

	return (found ? ExitStatus::Success : ExitStatus::NotFound);
    }
} /* namespace mqmx */
