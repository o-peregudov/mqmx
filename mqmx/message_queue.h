#pragma once

#include <mqmx/message.h>

#include <crs/mutex.h>

#include <deque>
#include <type_traits>

namespace mqmx
{
    /**
     * \brief Message queue (FIFO).
     *
     * In addition to the classical implementation of message queue this class
     * also supports a single listener (observer). And if this listener is set,
     * message queue will send notifications to it about some changes in state
     * of the object. Currently only three notifications supported and they are
     * described by the \link mqmx::message_queue::notification_flag \endlink
     * enumerator.
     *
     * \note All notifications are used for the purpose of internal implementation,
     *       but you may find them usefull for some other purposes.
     */
    class message_queue
    {
        message_queue (const message_queue &) = delete;
        message_queue & operator = (const message_queue &) = delete;

    public:
        typedef std::unique_ptr<message_queue>     upointer_type;
        typedef crs::mutex_type                    mutex_type;
        typedef crs::lock_type                     lock_type;
        typedef std::deque<message::upointer_type> container_type;
        typedef size_t                             notification_flags_type;

        enum notification_flag
        {
            data     = 0x0001, /*!< push operation called on this queue */
            detached = 0x0002, /*!< move ctor or move assignment called on this queue
                                * and the queue is no longer usable
                                */
            closed   = 0x0004  /*!< destructor called on this queue */
        };

        /**
         * \brief Interface for the listener.
         *
         * Provides the way for passing notifications about changes in state
         * of message queue object.
         */
        struct listener
        {
            virtual ~listener () { }
            virtual void notify (const queue_id_type,
                                 message_queue *,
                                 const notification_flags_type) = 0;
        };

    public:
        /**
         * \brief Default constructor.
         */
        message_queue (const queue_id_type = message::undefined_qid);

        /**
         * \brief Destructor.
         *
         * In case some listener is set for this message queue it will be notified
	 * with \link mqmx::message_queue::notification_flag::closed \endlink
         * notification.
         */
        ~message_queue ();

        /**
         * \brief Move constructor.
         *
         * \note The listener set in the original message queue is not moved, instead
         *       \link mqmx::message_queue::notification_flag::detached \endlink
         *       notification will be delivered to it.
         */
        message_queue (message_queue &&);

        /**
         * \brief Move assignment.
         *
         * \note The listener set in the original message queue is not moved, instead
         *       \link mqmx::message_queue::notification_flag::detached \endlink
         *       notification will be delivered to it.
         */
        message_queue & operator = (message_queue &&);

        /**
         * \brief Get ID of this message queue.
         */
        queue_id_type get_qid () const;

        /**
         * \brief Push some message to the end of the queue.
         *
         * This method also delivers \link mqmx::message_queue::notification_flag::data \endlink
         * notification in case some listener is set, but only if before this call message queue
         * was empty. So only first push will be reported to the listener.
         *
         * \note The object of this class could be moved out and in this case push
         *       operation will fail with status code ExitStatus::NotSupported.
         *
         * \retval ExitStatus::Success          if operation completed successfully
         * \retval ExitStatus::InvalidArgument  if argument is a nullptr
         * \retval ExitStatus::NotSupported     if message passed as a parameter doesn't belong
         *                                      to this message queue (has different QID) or
         *                                      message queue was moved out
         */
        status_code push (message::upointer_type &&);

        /**
         * \brief Remove and return message from the top of the queue.
         *
         * \returns Pointer to the message or nullptr if queue is empty or moved out.
         */
        message::upointer_type pop ();

        /**
         * \brief Create a message for this particular queue.
         *
         * \returns Pointer to a newly created message
         */
        template <typename message_type, typename... parameters>
        message::upointer_type new_message (parameters&&... args) const
        {
            static_assert (std::is_base_of<message, message_type>::value,
                           "Invalid message_type - should be derived from mqmx::message");
            return message::upointer_type (
                new message_type (get_qid (), std::forward<parameters> (args)...));
        }

        /**
         * \brief Create and push newly created message to the end of message queue.
         *
         * \returns The same set of status codes that could be returned from the
         *          \link mqmx::message_queue::push \endlink method
         */
        template <typename message_type, typename... parameters>
        status_code enqueue (parameters&&... args)
        {
            static_assert (std::is_base_of<message, message_type>::value,
                           "Invalid message_type - should be derived from mqmx::message");
            return push (new_message<message_type> (std::forward<parameters> (args)...));
        }

    public:
        /**
         * \Brief Sets new listener for this message queue.
         *
         * By design only one listener can be set.
         *
         * \note If message queue has some messages the newly set listener will get
         * \link mqmx::message_queue::notification_flag::data \endlink notification
         * immediately from this call.
         *
         * \retval ExitStatus::Success       if operation completed successfully
         * \retval ExitStatus::AlreadyExist  if listener is already set
         */
        status_code set_listener (listener &);

        /**
         * \brief Removes listener.
         */
        void clear_listener ();

    private:
        queue_id_type  _id;
        mutex_type     _mutex;
        container_type _queue;
        listener *     _listener;
    };
} /* namespace mqmx */
