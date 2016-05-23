#pragma once

#include <mqmx/message_queue.h>
#include <mqmx/WaitTimeProvider.h>

#include <Common/OAMThreading.hpp>

#include <algorithm>
#include <vector>
#include <cassert>
#include <iterator>
#include <tuple>

namespace mqmx
{
    /**
     * \brief Class for polling several message queues for notifications.
     *
     * Class itself is lightweight, and the main implementation is done in
     * templated method - poll. Example use of this class is the following:
     *
     *    mqmx::MessageQueuePoll mqp;
     *    const auto signaled_queues = mqp.poll (std::begin (mq2poll), std::end (mq2poll), timeout);
     *
     */
    class MessageQueuePoll final : message_queue::listener
    {
        MessageQueuePoll (const MessageQueuePoll &) = delete;
        MessageQueuePoll & operator = (const MessageQueuePoll &) = delete;

    public:
        typedef message_queue::mutex_type mutex_type;
        typedef message_queue::lock_type  lock_type;
        typedef BBC_pkg::oam_condvar_type condvar_type;

        class notification_rec_type
            : private std::tuple<queue_id_type, message_queue *, message_queue::notification_flags_type>
        {
            typedef std::tuple<queue_id_type,
                               message_queue *,
                               message_queue::notification_flags_type> base;

        public:
            notification_rec_type (queue_id_type qid,
                                   message_queue * mq,
                                   message_queue::notification_flags_type flags)
                : base (qid, mq, flags)
            { }

            notification_rec_type () = default;
            notification_rec_type (const notification_rec_type &) = default;
            notification_rec_type & operator = (const notification_rec_type &) = default;

            notification_rec_type (notification_rec_type &&) = default;
            notification_rec_type & operator = (notification_rec_type &&) = default;

            queue_id_type & getQID ()
            {
                return std::get<0> (*this);
            }

            message_queue * & getMQ ()
            {
                return std::get<1> (*this);
            }

            message_queue::notification_flags_type & getFlags ()
            {
                return std::get<2> (*this);
            }

            queue_id_type const & getQID () const
            {
                return std::get<0> (*this);
            }

            message_queue * const & getMQ () const
            {
                return std::get<1> (*this);
            }

            message_queue::notification_flags_type const & getFlags () const
            {
                return std::get<2> (*this);
            }
        };

        typedef std::vector<notification_rec_type> notifications_list_type;

    private:
        mutable mutex_type      m_notifications_mutex;
        condvar_type            m_notifications_condition;
        notifications_list_type m_notifications;

        virtual void notify (const queue_id_type,
                             message_queue *,
                             const message_queue::notification_flags_type) override;

        template <typename RefClockProvider>
        void waitForNotifications (const WaitTimeProvider & wtp, const RefClockProvider & rcp)
        {
            lock_type notifications_guard (m_notifications_mutex);
            const auto abs_time = wtp.getTimepoint (rcp);
            if (m_notifications.empty ())
            {
                const auto pred = [&]{ return !m_notifications.empty (); };
                if (wtp.waitInfinitely ())
                {
                    m_notifications_condition.wait (notifications_guard, pred);
                }
                else if (abs_time.time_since_epoch ().count () != 0)
                {
                    m_notifications_condition.wait_until (
                        notifications_guard, abs_time, pred);
                }
            }
        }

    public:
        MessageQueuePoll ();
        virtual ~MessageQueuePoll ();

        /*
         * NOTE: iterators should represent a sequence of pointers to MessageQueue
         */
        template <typename ForwardIt,
                  typename RefClockProvider = WaitTimeProvider>
        notifications_list_type poll (const ForwardIt ibegin, const ForwardIt iend,
                                      const WaitTimeProvider & wtp = WaitTimeProvider (),
                                      const RefClockProvider & rcp = WaitTimeProvider ())
        {
            {
                /*
                 * initialize list of notifications
                 */
                lock_type notifications_guard (m_notifications_mutex);
                m_notifications.clear ();
            }

            std::for_each (ibegin, iend,
                           [&](typename std::iterator_traits<ForwardIt>::reference mq)
                           {
                               const status_code ret_code = mq->set_listener (*this);
                               assert (ret_code == ExitStatus::Success), ret_code;
                           });
            waitForNotifications (wtp, rcp);
            std::for_each (ibegin, iend,
                           [&](typename std::iterator_traits<ForwardIt>::reference mq)
                           {
                               mq->clear_listener ();
                           });
            return m_notifications;
        }
    };
} /* namespace mqmx */
