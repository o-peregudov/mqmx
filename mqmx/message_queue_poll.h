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
    class message_queue_poll_listener : public message_queue::listener
    {
        message_queue_poll_listener (const message_queue_poll_listener &) = delete;
        message_queue_poll_listener & operator = (const message_queue_poll_listener &) = delete;

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

            bool operator < (const notification_rec_type & r) const
            {
                return (get_qid () < r.get_qid ());
            }

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

    public:
        message_queue_poll_listener ();
        virtual ~message_queue_poll_listener ();

        template <typename reference_clock_provider>
        void wait_for_notifications (const wait_time_provider & wtp,
                                     const reference_clock_provider & rcp)
        {
            lock_type guard (_mutex);
            if (_notifications.empty ())
            {
                const auto pred = [&]{ return !_notifications.empty (); };
                if (wtp.wait_infinitely ())
                {
                    _condition.wait (guard, pred);
                    return;
                }

                const auto abs_time = wtp.get_time_point (rcp);
                if (abs_time.time_since_epoch ().count () != 0)
                {
                    _condition.wait_until (guard, abs_time, pred);
                }
            }
        }

        notifications_list_type get_notifications () const
        {
            lock_type guard (_mutex);
            return _notifications;
        }
    };

    /*
     * NOTE: iterators should represent a sequence of pointers to message_queue
     */
    template <typename forward_it,
              typename reference_clock_provider = wait_time_provider>
    message_queue_poll_listener::notifications_list_type
    poll (const forward_it ibegin, const forward_it iend,
          const wait_time_provider & wtp = wait_time_provider (),
          const reference_clock_provider & rcp = wait_time_provider ())
    {
        message_queue_poll_listener listener;
        std::for_each (ibegin, iend,
                       [&listener](typename std::iterator_traits<forward_it>::reference mq)
                       {
                           const status_code ret_code = mq->set_listener (listener);
                           assert (ret_code == ExitStatus::Success), ret_code;
                       });
        listener.wait_for_notifications (wtp, rcp);
        std::for_each (ibegin, iend,
                       [&listener](typename std::iterator_traits<forward_it>::reference mq)
                       {
                           mq->clear_listener ();
                       });
        return listener.get_notifications ();
    }
} /* namespace mqmx */
