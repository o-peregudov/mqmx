#pragma once

#include <mqmx/MessageQueue.h>
#include <mqmx/WaitTimeProvider.h>

#include <Common/OAMThreading.hpp>

#include <algorithm>
#include <vector>
#include <cassert>
#include <iterator>
#include <tuple>

namespace mqmx
{
    class MessageQueuePoll final : MessageQueue::Listener
    {
        MessageQueuePoll (const MessageQueuePoll &) = delete;
        MessageQueuePoll & operator = (const MessageQueuePoll &) = delete;

    public:
        typedef MessageQueue::mutex_type                  mutex_type;
        typedef MessageQueue::lock_type                   lock_type;
        typedef BBC_pkg::oam_condvar_type                 condvar_type;
        typedef std::tuple<queue_id_type, MessageQueue *,
                           MessageQueue::notification_flags_type>
                                                          notification_rec_type;
        typedef std::vector<notification_rec_type>        notifications_list_type;

    private:
        mutex_type              _poll_mutex;
        mutable mutex_type      _notifications_mutex;
        condvar_type            _notifications_condition;
        notifications_list_type _notifications;

        virtual void notify (const queue_id_type,
                             MessageQueue *,
                             const MessageQueue::notification_flags_type) override;

    public:
        MessageQueuePoll ();
        virtual ~MessageQueuePoll ();

        /*
         * NOTE: iterators should represent a sequence of pointers to message_queue
         */
        template <typename ForwardIt,
                  typename RefClockProvider = WaitTimeProvider>
        notifications_list_type poll (const ForwardIt ibegin, const ForwardIt iend,
                                      const WaitTimeProvider & wtp = WaitTimeProvider (),
                                      const RefClockProvider & rcp = WaitTimeProvider ())
        {
            lock_type poll_guard (_poll_mutex); /* to block re-entrance */
            {
                /*
                 * initialize list of notifications
                 */
                lock_type notifications_guard (_notifications_mutex);
                _notifications.clear ();
            }

            /*
             * set listeners for each message queue
             */
            std::for_each (ibegin, iend,
                           [&](typename std::iterator_traits<ForwardIt>::reference mq)
                           {
                               const status_code ret_code = mq->setListener (*this);
                               assert (ret_code == ExitStatus::Success);
                           });

            /*
             * wait for notifications
             */
            lock_type notifications_guard (_notifications_mutex);
            const auto abs_time = wtp.getTimepoint (rcp);
            if (_notifications.empty ())
            {
                const auto pred = [&]{ return !_notifications.empty (); };
                if (wtp.waitInfinitely ())
                {
                    _notifications_condition.wait (notifications_guard, pred);
                }
                else if (abs_time.time_since_epoch ().count () != 0)
                {
                    _notifications_condition.wait_until (
                        notifications_guard, abs_time, pred);
                }
            }

            /*
             * remove listeners from each message queue
             */
            std::for_each (ibegin, iend,
                           [&](typename std::iterator_traits<ForwardIt>::reference mq)
                           {
                               mq->clearListener ();
                           });
            return _notifications;
        }
    };
} /* namespace mqmx */
