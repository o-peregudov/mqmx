#pragma once

#include <mqmx/libexport.h>
#include <mqmx/types.h>
#include <memory>

namespace mqmx
{
    /**
     * \brief Base class for all messages.
     *
     * Each message holds two attributes - message queue ID and message ID.
     * First is needed to specify to which queue this message belongs and because
     * of this there is no possibility to put message from one queue to the queue with
     * different ID. Second - provides the information for proper message deserialization.
     */
    class MQMX_EXPORT message
    {
        const queue_id_type _qid;
        const message_id_type _mid;

    public:
        typedef std::unique_ptr<message> upointer_type;

        static constexpr queue_id_type undefined_qid = static_cast<queue_id_type> (-1);

        message (const queue_id_type queue_id,
                 const message_id_type message_id)
            : _qid (queue_id)
            , _mid (message_id)
        {
        }

        virtual ~message ()
        {
        }

        /**
         * \returns ID of the queue to which this message belongs
         */
        queue_id_type get_qid () const
        {
            return _qid;
        }

        /**
         * \returns ID of the message
         */
        message_id_type get_mid () const
        {
            return _mid;
        }
    };
} /* namespace mqmx */
