#include <mqmx/message_queue_pool.h>
#include <crs/semaphore.h>
#include <gmock/gmock.h>

TEST (message_queue_pool, sanity_checks)
{
    mqmx::MessageQueuePool sut;
    ASSERT_TRUE (sut.isPollIdle ());

    auto mq = sut.allocateQueue (mqmx::MessageQueuePool::message_handler_func_type ());
    ASSERT_EQ (nullptr, mq.get ());

    mq = sut.allocateQueue ([](mqmx::message::upointer_type && msg)->mqmx::status_code
                            {
                                return mqmx::ExitStatus::Success;
                            });
    ASSERT_NE (nullptr, mq.get ());
}

TEST (message_queue_pool, counter_test)
{
    const size_t NMSGS = 1000;
    CrossClass::semaphore sem;
    mqmx::MessageQueuePool sut;
    ASSERT_TRUE (sut.isPollIdle ());
    size_t counter_a = 0;
    size_t counter_b = 0;
    size_t counter_c = 0;
    auto mqa = sut.allocateQueue (
	[&](mqmx::message::upointer_type && msg)
	{
	    if (++counter_a == NMSGS)
	    {
		sem.post ();
	    }
	    return mqmx::ExitStatus::Success;
	});
    auto mqb = sut.allocateQueue (
    	[&](mqmx::message::upointer_type && msg)
    	{
    	    if (++counter_b == NMSGS)
    	    {
    		sem.post ();
    	    }
    	    return mqmx::ExitStatus::Success;
    	});
    auto mqc = sut.allocateQueue (
	[&](mqmx::message::upointer_type && msg)
	{
	    if (++counter_c == NMSGS)
	    {
		sem.post ();
	    }
	    return mqmx::ExitStatus::Success;
	});
    {
	std::thread threada ([&](){
		for (size_t i = NMSGS; 0 < i; --i)
		{
		    mqa->enqueue<mqmx::message> (i);
		}
	    });
	std::thread threadb ([&](){
		for (size_t i = NMSGS; 0 < i; --i)
		{
		    mqb->enqueue<mqmx::message> (i);
		}
	    });
	std::thread threadc ([&](){
		for (size_t i = NMSGS; 0 < i; --i)
		{
		    mqc->enqueue<mqmx::message> (i);
		}
	    });
	threada.join ();
	threadb.join ();
	threadc.join ();
    }
    sem.wait ();
    sem.wait ();
    sem.wait ();
    EXPECT_EQ (NMSGS, counter_a);
    EXPECT_EQ (NMSGS, counter_b);
}
