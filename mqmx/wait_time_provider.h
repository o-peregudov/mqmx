#pragma once

#include <mqmx/libexport.h>
#include <chrono>

namespace mqmx
{
    /**
     * \brief Class for specifying infinite time intervals.
     */
    struct MQMX_EXPORT infinite_wait_time {};

    /**
     * \brief Checks, whether time point is empty (set to epoch).
     *
     * \retval true if time point is set to epoch (empty)
     * \retval false otherwise
     */
    template <typename Clock, typename Duration = typename Clock::duration>
    bool is_time_point_empty (const std::chrono::time_point<Clock, Duration> & tp)
    {
        return (0 == tp.time_since_epoch ().count ());
    }

    /**
     * \brief Auxiliary class for handling wait time intervals.
     *
     * Class has a couple of converting constructors and therefore it could be
     * used as a convenience type for timeout parameters. After creation objects of this
     * class can calculate an absolute time point until which the wait should be performed.
     *
     * This clas also supports a special infinite wait interval.
     */
    class MQMX_EXPORT wait_time_provider final
    {
    public:
        typedef std::chrono::steady_clock clock_type;
        typedef clock_type::duration      duration_type;
        typedef clock_type::time_point    time_point_type;

        static const infinite_wait_time WAIT_INFINITELY; //!< Constant for infinite wait interval

    private:
        const duration_type   _rel_time;
        const time_point_type _abs_time;
        const bool            _wait_infinitely;

        time_point_type get_current_time_point () const
        {
            return clock_type::now ();
        }

    public:
        /**
         * \brief Default constructor.
         *
         * No wait interval is specified with this constructor.
         */
        wait_time_provider ()
            : _rel_time ()
            , _abs_time ()
            , _wait_infinitely (false)
        { }

        /**
         * \brief Dedicated converting constructor for infinite wait intervals.
         */
        wait_time_provider (const infinite_wait_time &)
            : _rel_time ()
            , _abs_time ()
            , _wait_infinitely (true)
        { }

        /**
         * \brief Converting constructor for various (relative) durations.
         */
        template <class Rep, class Period>
        wait_time_provider (const std::chrono::duration<Rep, Period> & rel_time)
            : _rel_time (rel_time)
            , _abs_time ()
            , _wait_infinitely (false)
        { }

        /**
         * \brief Converting constructor for various (absolute) time points.
         */
        template <class Clock, class Duration>
        wait_time_provider (const std::chrono::time_point<Clock, Duration> & abs_time)
            : _rel_time ()
            , _abs_time (abs_time)
            , _wait_infinitely (false)
        { }

        /**
         * \brief Getter for infinite wait interval state.
         *
         * \returns true if infinite wait interval is set and false otherwise
         */
        bool wait_infinitely () const
        {
            return _wait_infinitely;
        }

        /**
         * \brief Getter for time point (simple).
         *
         * If wait time is specified as a relative time (duration), than this
         * method calculates absolute time point as (current-time + duration).
         *
         * If wait time is specified as an abslute time (time point), than this
         * exactly time point is returned withour any modifications.
         */
        time_point_type get_time_point () const
        {
            return get_time_point (*this);
        }

        /**
         * \brief Getter for time point (generic).
         *
         * This version of getter uses custom reference time provider for getting
         * current-time. So in general it provides the way for modifications of
         * time point for waiting in case it is specified as a relative time (duration).
         *
         * This extra functionality could be used, for example, in tests, where
         * precies control of timeouts is required.
         */
        template <class reference_clock_provider>
        time_point_type get_time_point (const reference_clock_provider & rcp) const
        {
            if (is_time_point_empty (_abs_time) && (_rel_time.count () == 0))
            {
                return time_point_type ();
            }

            return (is_time_point_empty (_abs_time)
                    ? (rcp.get_current_time_point () + _rel_time)
                    : _abs_time);
        }
    };
} /* namespace mqmx */
