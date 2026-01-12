#pragma once

#include <pthread.h>
#include <thread>
#include <iostream>
#ifdef __linux__
void pin_thread(std::thread &t, int core_id)
{
    cpu_set_t cpu_set;

    CPU_ZERO(&cpu_set);
    CPU_SET(core_id, &cpu_set);

    int result = pthread_setaffinity_np(t.native_handle(), sizeof(cpu_set_t), &cpu_set);

    if (result != 0)
    {
        std::cerr << "Can't set affinity for thread " << t.get_id() << '\n';
    }
}

#elifdef _WIN32

// For win32 api

#elifdef __APPLE__
// for mac implementation

#endif
