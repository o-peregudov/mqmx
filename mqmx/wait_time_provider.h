#ifndef MQMX_WAIT_TIME_PROVIDER_H_INCLUDED
#define MQMX_WAIT_TIME_PROVIDER_H_INCLUDED 1

#include <chrono>

namespace mqmx
{
    struct WAIT_INFINITELY {};

    class wait_time_provider final
    {
    public:
	typedef std::chrono::steady_clock clock_type;
	typedef clock_type::duration      duration_type;
	typedef clock_type::time_point    time_point_type;

    private:
	const duration_type _rel_time;
	const time_point_type _abs_time;
	const bool _wait_infinitely;

	time_point_type get_now () const
	{
	    return clock_type::now ();
	}

    public:
	wait_time_provider ()
	    : _rel_time ()
	    , _abs_time ()
	    , _wait_infinitely (false)
	{ }

	wait_time_provider (const WAIT_INFINITELY &)
	    : _rel_time ()
	    , _abs_time ()
	    , _wait_infinitely (true)
	{ }

	wait_time_provider (const duration_type & rel_time)
	    : _rel_time (rel_time)
	    , _abs_time ()
	    , _wait_infinitely (false)
	{ }

	wait_time_provider (const time_point_type & abs_time)
	    : _rel_time ()
	    , _abs_time (abs_time)
	    , _wait_infinitely (false)
	{ }

	bool wait_infinitely () const
	{
	    return _wait_infinitely;
	}

	time_point_type get_time_point () const
	{
	    return get_time_point (*this);
	}

	template <class reference_clock_provider>
	time_point_type get_time_point (const reference_clock_provider & rcp) const
	{
	    if ((_abs_time.time_since_epoch ().count () == 0) &&
		(_rel_time.count () == 0))
	    {
		return time_point_type ();
	    }

	    return ((_abs_time.time_since_epoch ().count () == 0)
		    ? (rcp.get_now () + _rel_time)
		    : _abs_time);
	}
    };
} /* namespace mqmx */
#endif /* MQMX_WAIT_TIME_PROVIDER_H_INCLUDED */
