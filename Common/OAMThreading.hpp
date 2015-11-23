#pragma once
#include <thread>
#include <condition_variable>

namespace BBC_pkg
{
    typedef std::thread             oam_thread_type;
    typedef std::condition_variable oam_condvar_type;
} /* namespace BBC_pkg */
