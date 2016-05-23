#pragma once

#include <chrono>

namespace mqmx
{
    struct infinite_wait_time {};

    class wait_time_provider final
    {
    public:
        typedef std::chrono::steady_clock clock_type;
        typedef clock_type::duration      duration_type;
        typedef clock_type::time_point    time_point_type;

        static const infinite_wait_time WAIT_INFINITELY;

    private:
        const duration_type   _rel_time;
        const time_point_type _abs_time;
        const bool            _wait_infinitely;

        time_point_type get_current_timepoint () const
        {
            return clock_type::now ();
        }

    public:
        wait_time_provider ()
            : _rel_time ()
            , _abs_time ()
            , _wait_infinitely (false)
        { }

        wait_time_provider (const infinite_wait_time &)
            : _rel_time ()
            , _abs_time ()
            , _wait_infinitely (true)
        { }

        template <class Rep, class Period>
        wait_time_provider (const std::chrono::duration<Rep, Period> & rel_time)
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

        time_point_type get_timepoint () const
        {
            return get_timepoint (*this);
        }

        template <class reference_clock_provider>
        time_point_type get_timepoint (const reference_clock_provider & rcp) const
        {
            if ((_abs_time.time_since_epoch ().count () == 0) &&
                (_rel_time.count () == 0))
            {
                return time_point_type ();
            }

            return ((_abs_time.time_since_epoch ().count () == 0)
                    ? (rcp.get_current_timepoint () + _rel_time)
                    : _abs_time);
        }
    };
} /* namespace mqmx */
