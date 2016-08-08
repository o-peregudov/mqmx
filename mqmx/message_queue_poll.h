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

namespace mqmx
{
    /**
     * \brief Listener for polling state of multiple message queues.
     *
     * Special listener for polling notifications from multiple message queues.
     * Stores all notifications as a list (vector) in ascending order of message
     * queue ID, so record about notifications from message queue with the
     * smallest ID will be first in the list.
     *
     * This class doesn't poll message queues directly, but polling is done
     * when this listener is set for some message queue.
     * \see \link mqmx::message_queue::set_listener \endlink
     *
     * Class also provides the way for waiting for notifications for some time.
     */
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
        /**
         * \brief Default constructor.
         */
        message_queue_poll_listener ();

        /**
         * \brief Destructor.
         */
        virtual ~message_queue_poll_listener ();

        /**
         * \brief Get the list of notifications.
         */
        notifications_list_type get_notifications () const
        {
            lock_type guard (_mutex);
            return _notifications;
        }

        /**
         * \brief Wait for notifications.
         *
         * If the current list of notifications is empty method will block
         * until either timeout expires or any notification delivered.
         */
        template <typename reference_clock_provider>
        notifications_list_type wait_for_notifications (const wait_time_provider & wtp,
                                                        const reference_clock_provider & rcp)
        {
            lock_type guard (_mutex);
            if (_notifications.empty ())
            {
                const auto pred = [&]{ return !_notifications.empty (); };
                if (wtp.wait_infinitely ())
                {
                    _condition.wait (guard, pred);
                }
                else
                {
                    const auto abs_time = wtp.get_time_point (rcp);
                    if (abs_time.time_since_epoch ().count () != 0)
                    {
                        _condition.wait_until (guard, abs_time, pred);
                    }
                }
            }
            return _notifications;
        }
    };

    /**
     * \brief Helper class for set listener to multiple queues in RAII way.
     *
     * Sets given listener for each message queue from the range defined by
     * two iterators. Iterators should represent a sequence of pointers to
     * objects of class \link mqmx::message_queue \endlink.
     */
    template <typename ForwardIt>
    class set_listener_for_each_queue
    {
        ForwardIt _first;
        ForwardIt _last;

        set_listener_for_each_queue (const set_listener_for_each_queue &) = delete;
        set_listener_for_each_queue & operator = (const set_listener_for_each_queue &) = delete;

        static_assert (std::is_base_of<std::forward_iterator_tag,
                       typename std::iterator_traits<ForwardIt>::iterator_category>::value,
                       "ForwardIterator should be used");

    public:
        /**
         * \brief Constructor.
         *
         * Sets a given listener for each message queue from the range [first, last).
         */
        set_listener_for_each_queue (ForwardIt first,
                                     ForwardIt last,
                                     mqmx::message_queue::listener & listener)
            : _first (first)
            , _last (last)
        {
            std::for_each (_first, _last,
                           [&listener](typename std::iterator_traits<ForwardIt>::reference mq)
                           {
                               const status_code ret_code = mq->set_listener (listener);
                               assert (ret_code == ExitStatus::Success), ret_code;
                           });
        }

        /**
         * \brief Destructor.
         *
         * Clears listener for each message queue from the range [first, last).
         */
        ~set_listener_for_each_queue ()
        {
            std::for_each (_first, _last,
                           [](typename std::iterator_traits<ForwardIt>::reference mq)
                           {
                               mq->clear_listener ();
                           });
        }
    };

    /**
     * \brief Function waits for the notifications on multiple message queues.
     *
     * \note Iterators should represent a sequence of pointers to objects of
     *       class \link mqmx::message_queue \endlink.
     *
     * \returns The list of notifications records for message queues for which
     *          any notifications were reported.
     */
    template <typename ForwardIt,
              typename reference_clock_provider = wait_time_provider>
    message_queue_poll_listener::notifications_list_type
    poll (const ForwardIt first,
          const ForwardIt last,
          const wait_time_provider & wtp = wait_time_provider (),
          const reference_clock_provider & rcp = wait_time_provider ())
    {
        message_queue_poll_listener listener;
        set_listener_for_each_queue<ForwardIt> set (first, last, listener);
        return listener.wait_for_notifications (wtp, rcp);
    }
} /* namespace mqmx */
