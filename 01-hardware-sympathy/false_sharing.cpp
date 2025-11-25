/**
 * @file false_sharing.cpp
 * @brief Demonstrates the performance impact of False Sharing in multithreaded C++.
 * @author thep1ckaxe
 * * CONCEPT:
 * Modern CPUs load memory in "Cache Lines" (typically 64 bytes).
 * If two threads modify different variables that happen to sit on the SAME
 * cache line, the CPU cores must constantly exchange ownership of that line.
 * This "ping-pong" effect drastically reduces performance.
 * * FIX:
 * Use `alignas(64)` to force variables onto separate cache lines.
 */

#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
#include <iomanip>
#include <new>

#ifdef __cpp_lib_hardware_interference_size
using std::hardware_destructive_interference_size;
#else
constexpr std::size_t hardware_destructive_interference_size = 64;
#endif

using Clock = std::chrono::system_clock;

constexpr int ITERATIONS = 1e7;
constexpr int SIMULATIONS = 1000;

struct Unpadded
{
    std::atomic_int a{0};
    std::atomic_int b{0};
};

struct Padded
{
    std::atomic_int a{0};
    alignas(hardware_destructive_interference_size) std::atomic_int b{0}; // 64 bytes alignment
};

template <typename T>
double run_benchmark(const std::string_view name)
{
    T data;

    auto start = Clock::now();

    std::thread t1([&]()
                   {
        for (int i = 0; i < ITERATIONS; i++)
        {
            data.a.fetch_add(1, std::memory_order_relaxed);
        } });

    std::thread t2([&]()
                   {
        for (int i = 0; i < ITERATIONS; i++)
        {
            data.b.fetch_add(1, std::memory_order_relaxed);
        } });

    t1.join(), t2.join();

    auto end = Clock::now();

    std::chrono::duration<double> diff = end - start;

    // std::cout << "Struct: " << name << '\n'
    //           << " | Time: " << diff.count() << "s\n";
    return diff.count();
}

int main()
{
    std::cout << "Align of Unpadded struct: " << alignof(Unpadded) << "\n";
    std::cout << "Align of Padded struct: " << alignof(Padded) << "\n";

    std::cout << "Size of Unpadded struct: " << sizeof(Unpadded) << "\n";
    std::cout << "Size of Padded struct: " << sizeof(Padded) << "\n";

    double avg_unpadded_time = 0;
    double min_unpadded_time = 100;
    double max_unpadded_time = 0;
    double avg_padded_time = 0;
    double min_padded_time = 100;
    double max_padded_time = 0;

    for (int i = 0; i < SIMULATIONS; i++)
    {
        double cost = run_benchmark<Unpadded>("Unpadded");
        avg_unpadded_time += cost;
        min_unpadded_time = std::min(min_unpadded_time, cost);
        max_unpadded_time = std::max(max_unpadded_time, cost);

    }
    avg_unpadded_time /= SIMULATIONS;

    for (int i = 0; i < SIMULATIONS; i++)
    {
        double cost = run_benchmark<Padded>("Padded");
        avg_padded_time += cost;
        min_padded_time = std::min(min_padded_time, cost);
        max_padded_time = std::max(max_padded_time, cost);
    }
    avg_padded_time /= SIMULATIONS;

    std::cout << "Over " << SIMULATIONS << " benchmark run each struct:\n";
    std::cout << "Avg Performance diff: " << avg_unpadded_time / avg_padded_time << "x faster\n";
    std::cout << "Max Performance diff: " << max_unpadded_time / max_padded_time << "x faster\n";
    std::cout << "Min Performance diff: " << min_unpadded_time / min_padded_time << "x faster\n";
}