#pragma once

#include <mqmx/types.h>
#include <memory>

namespace mqmx
{
    class message
    {
        const queue_id_type _qid;
        const message_id_type _mid;

    public:
	typedef std::unique_ptr<message> upointer_type;

        static constexpr queue_id_type UndefinedQID = static_cast<queue_id_type> (-1);

        message (const queue_id_type queue_id,
                 const message_id_type message_id)
            : _qid (queue_id)
            , _mid (message_id)
        {
        }

        virtual ~message ()
        {
        }

        queue_id_type get_qid () const
        {
            return _qid;
        }

        message_id_type get_mid () const
        {
            return _mid;
        }
    };
} /* namespace mqmx */
