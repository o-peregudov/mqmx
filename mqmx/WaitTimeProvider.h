#pragma once

#include <chrono>

namespace mqmx
{
    struct InfiniteWaitTime {};

    class WaitTimeProvider final
    {
    public:
        typedef std::chrono::steady_clock clock_type;
        typedef clock_type::duration      duration_type;
        typedef clock_type::time_point    time_point_type;

        static const InfiniteWaitTime WAIT_INFINITELY;

    private:
        const duration_type   _rel_time;
        const time_point_type _abs_time;
        const bool            _wait_infinitely;

        time_point_type getCurrentTimepoint () const
        {
            return clock_type::now ();
        }

    public:
        WaitTimeProvider ()
            : _rel_time ()
            , _abs_time ()
            , _wait_infinitely (false)
        { }

        WaitTimeProvider (const InfiniteWaitTime &)
            : _rel_time ()
            , _abs_time ()
            , _wait_infinitely (true)
        { }

        template <class Rep, class Period>
        WaitTimeProvider (const std::chrono::duration<Rep, Period> & rel_time)
            : _rel_time (rel_time)
            , _abs_time ()
            , _wait_infinitely (false)
        { }

        WaitTimeProvider (const time_point_type & abs_time)
            : _rel_time ()
            , _abs_time (abs_time)
            , _wait_infinitely (false)
        { }

        bool waitInfinitely () const
        {
            return _wait_infinitely;
        }

        time_point_type getTimepoint () const
        {
            return getTimepoint (*this);
        }

        template <class reference_clock_provider>
        time_point_type getTimepoint (const reference_clock_provider & rcp) const
        {
            if ((_abs_time.time_since_epoch ().count () == 0) &&
                (_rel_time.count () == 0))
            {
                return time_point_type ();
            }

            return ((_abs_time.time_since_epoch ().count () == 0)
                    ? (rcp.getCurrentTimepoint () + _rel_time)
                    : _abs_time);
        }
    };
} /* namespace mqmx */
