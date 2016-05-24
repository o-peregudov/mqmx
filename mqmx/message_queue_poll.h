#pragma once

#include <mqmx/message_queue.h>
#include <mqmx/wait_time_provider.h>

#include <crs/mutex.h>
#include <crs/condition_variable.h>

#include <algorithm>
#include <iterator>
#include <cassert>
#include <vector>
#include <tuple>

namespace crs = CrossClass;
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
        typedef crs::mutex_type   mutex_type;
        typedef crs::lock_type    lock_type;
        typedef crs::condvar_type condvar_type;

        class notification_rec_type : std::tuple<queue_id_type,
                                                 message_queue *,
                                                 message_queue::notification_flags_type>
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
        mutable mutex_type      _mutex;
        condvar_type            _condition;
        notifications_list_type _notifications;

        virtual void notify (const queue_id_type,
                             message_queue *,
                             const message_queue::notification_flags_type) override;

        template <typename reference_clock_provider>
        void wait_for_notifications (const wait_time_provider & wtp,
                                     const reference_clock_provider & rcp)
        {
            lock_type guard (_mutex);
            const auto abs_time = wtp.get_timepoint (rcp);
            if (_notifications.empty ())
            {
                const auto pred = [&]{ return !_notifications.empty (); };
                if (wtp.wait_infinitely ())
                {
                    _condition.wait (guard, pred);
                }
                else if (abs_time.time_since_epoch ().count () != 0)
                {
                    _condition.wait_until (guard, abs_time, pred);
                }
            }
        }

    public:
        message_queue_poll ();
        virtual ~message_queue_poll ();

        /*
         * NOTE: iterators should represent a sequence of pointers to MessageQueue
         */
        template <typename forward_it,
                  typename reference_clock_provider = wait_time_provider>
        notifications_list_type poll (const forward_it ibegin, const forward_it iend,
                                      const wait_time_provider & wtp = wait_time_provider (),
                                      const reference_clock_provider & rcp = wait_time_provider ())
        {
            {
                /*
                 * initialize list of notifications
                 */
                lock_type guard (_mutex);
                _notifications.clear ();
            }

            std::for_each (ibegin, iend,
                           [&](typename std::iterator_traits<forward_it>::reference mq)
                           {
                               const status_code ret_code = mq->set_listener (*this);
                               assert (ret_code == ExitStatus::Success), ret_code;
                           });
            wait_for_notifications (wtp, rcp);
            std::for_each (ibegin, iend,
                           [&](typename std::iterator_traits<forward_it>::reference mq)
                           {
                               mq->clear_listener ();
                           });
            return _notifications;
        }
    };
} /* namespace mqmx */
