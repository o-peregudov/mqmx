#pragma once

#include <mqmx/types.h>

namespace mqmx
{
    class Message
    {
        const queue_id_type _qid;
        const message_id_type _mid;

    public:
        static const queue_id_type UndefinedQID = static_cast<queue_id_type> (-1);

        Message (const queue_id_type queue_id,
                 const message_id_type message_id)
            : _qid (queue_id)
            , _mid (message_id)
        {
        }

        virtual ~Message ()
        {
        }

        queue_id_type getQID () const
        {
            return _qid;
        }

        message_id_type getMID () const
        {
            return _mid;
        }
    };
} /* namespace mqmx */
