#pragma once

#include <mqmx/message_queue.h>
#include <mqmx/WaitTimeProvider.h>

#include <algorithm>
#include <iterator>
#include <cassert>
#include <vector>
#include <tuple>
#include <condition_variable>

namespace mqmx
{
    /**
     * \brief Class for polling several message queues for notifications.
     *
     * Class itself is lightweight, and the main implementation is done in
     * templated method - poll. Example use of this class is the following:
     *
     *    mqmx::message_queue_poll mqp;
     *    const auto signaled_queues = mqp.poll (std::begin (mq2poll), std::end (mq2poll), timeout);
     *
     */
    class message_queue_poll final : message_queue::listener
    {
        message_queue_poll (const message_queue_poll &) = delete;
        message_queue_poll & operator = (const message_queue_poll &) = delete;

    public:
        typedef message_queue::mutex_type mutex_type;
        typedef message_queue::lock_type  lock_type;
        typedef std::condition_variable   condvar_type;

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

            queue_id_type & get_qid ()
            {
                return std::get<0> (*this);
            }

            message_queue * & get_mq ()
            {
                return std::get<1> (*this);
            }

            message_queue::notification_flags_type & get_flags ()
            {
                return std::get<2> (*this);
            }

            queue_id_type const & get_qid () const
            {
                return std::get<0> (*this);
            }

            message_queue * const & get_mq () const
            {
                return std::get<1> (*this);
            }

            message_queue::notification_flags_type const & get_flags () const
            {
                return std::get<2> (*this);
            }
        };

        typedef std::vector<notification_rec_type> notifications_list_type;

    private:
        mutable mutex_type      _notifications_mutex;
        condvar_type            _notifications_condition;
        notifications_list_type _notifications;

        virtual void notify (const queue_id_type,
                             message_queue *,
                             const message_queue::notification_flags_type) override;

        template <typename RefClockProvider>
        void wait_for_notifications (const WaitTimeProvider & wtp, const RefClockProvider & rcp)
        {
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
        }

    public:
        message_queue_poll ();
        virtual ~message_queue_poll ();

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
                lock_type notifications_guard (_notifications_mutex);
                _notifications.clear ();
            }

            std::for_each (ibegin, iend,
                           [&](typename std::iterator_traits<ForwardIt>::reference mq)
                           {
                               const status_code ret_code = mq->set_listener (*this);
                               assert (ret_code == ExitStatus::Success), ret_code;
                           });
            wait_for_notifications (wtp, rcp);
            std::for_each (ibegin, iend,
                           [&](typename std::iterator_traits<ForwardIt>::reference mq)
                           {
                               mq->clear_listener ();
                           });
            return _notifications;
        }
    };
} /* namespace mqmx */
