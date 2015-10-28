#include "mqmx/message_queue.h"
#include <cassert>
#include <tuple>
#include <vector>

int main (int argc, const char ** argv)
{
    using namespace mqmx;
    const queue_id_type defQID = 10;
    const message_id_type defMID = 10;

    /*
     * default constructor
     */
    message_queue queue (defQID);
    message_queue::message_ptr_type msg (queue.pop ());
    assert ((msg.get () == nullptr) &&
            ("Initially queue is empty"));

    class listener_mock : public message_queue::listener
    {
    public:
        typedef std::tuple<const queue_id_type,
                           message_queue *,
                           const MQNotification> notification_rec;
        typedef std::vector<notification_rec>    notification_list;

    private:
        notification_list _notifications;

    public:
        listener_mock ()
            : message_queue::listener ()
            , _notifications ()
        { }

        virtual ~listener_mock ()
        { }

        virtual void notify (const queue_id_type qid,
                             message_queue * mq,
                             const MQNotification nid) noexcept override
        {
            _notifications.push_back (std::make_tuple (qid, mq, nid));
        }

        const notification_list & get_notifications () const
        {
            return _notifications;
        }

        void clear_notifications ()
        {
            _notifications.clear ();
        }
    };

    listener_mock sample_listener;
    status_code retCode = queue.set_listener (sample_listener);
    assert ((retCode == ExitStatus::Success) &&
            ("Listener should be registered"));

    /*
     * push operation
     */
    retCode = queue.push (message_queue::message_ptr_type (new message (defQID, defMID)));
    assert ((retCode == ExitStatus::Success) &&
            ("Push should succeed"));
    assert ((sample_listener.get_notifications ().size () == 1) &&
	    ("Single notification should be delivered"));
    assert ((std::get<2> (sample_listener.get_notifications ().back ()) == MQNotification::NewMessage) &&
	    ("Proper notification should be delivered"));
    return 0;
}
