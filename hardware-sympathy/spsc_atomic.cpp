#include <iostream>
#include <vector>
#include <atomic>
#include <thread>
#include <optional>
#include <assert.h>
// headers
template <typename T>
class SPSC
{

    alignas(64) std::atomic<size_t> head;
    alignas(64) std::atomic<size_t> tail;

    std::vector<T> buffer;
    bool full;
public:
    SPSC(size_t capacity) : head(0), tail(0), buffer(capacity) {};

    bool push(const T &item)
    {
        if ((head + 1) % buffer.size() == tail.load(std::memory_order_acquire))
            return false;
        buffer[head] = item;
        head.store((head + 1) % buffer.size(), std::memory_order_release);
        return true;
    }

    std::optional<T> pop()
    {
        if (tail.load(std::memory_order_relaxed) == head.load(std::memory_order_acquire))
            return std::nullopt;

        T item = buffer[tail];
        tail.store((tail + 1) % buffer.size(), std::memory_order_release);
        return item;
    }
};

int main()
{
    std::cout << "Size of SPSC: " << sizeof(SPSC<int>) << '\n';

    int iteration;
    size_t q_size;
    std::cout << "Iteration: ";
    std::cin >> iteration;
    std::cout << "Queue size: ";
    std::cin >> q_size;
    SPSC<int> spsc{q_size};

    std::thread prod([&]()
                     {
            for(int i=0;i<iteration;i++)
                while(!spsc.push(i)){
                    std::this_thread::yield();
                } });

    std::thread cons([&]()
                     {
            for (int i = 0; i < iteration; i++)
            {
                auto val = spsc.pop();
                while(!val.has_value()) {std::this_thread::yield(); val = spsc.pop();}

                std::cout << val.value() << '\n';
            } });

    prod.join();
    cons.join();
}

/*
The first perf use:
        30,250,504      task-clock                       #    0.004 CPUs utilized             
                 5      context-switches                 #  165.287 /sec                      
                 4      cpu-migrations                   #  132.229 /sec                      
               144      page-faults                      #    4.760 K/sec                     
        88,180,782      cpu_atom/instructions/           #    0.95  insn per cycle              (36.58%)
       208,811,040      cpu_core/instructions/           #    1.65  insn per cycle              (14.90%)
        92,763,545      cpu_atom/cycles/                 #    3.067 GHz                         (25.75%)
       126,310,368      cpu_core/cycles/                 #    4.175 GHz                         (14.90%)
        19,992,323      cpu_atom/branches/               #  660.892 M/sec                       (35.66%)
        46,212,389      cpu_core/branches/               #    1.528 G/sec                       (14.90%)
           181,206      cpu_atom/branch-misses/          #    0.91% of all branches             (50.57%)
            26,832      cpu_core/branch-misses/          #    0.06% of all branches             (14.90%)
 #     25.5 %  tma_backend_bound      
                                                  #      1.8 %  tma_bad_speculation    
                                                  #     27.6 %  tma_frontend_bound     
                                                  #     45.1 %  tma_retiring             (14.90%)
 #      6.2 %  tma_bad_speculation    
                                                  #     31.4 %  tma_retiring             (70.29%)
 #     36.0 %  tma_backend_bound      
                                                  #     26.4 %  tma_frontend_bound       (80.20%)
        27,126,825      cpu_atom/L1-dcache-loads/        #  896.740 M/sec                       (74.18%)
        61,282,601      cpu_core/L1-dcache-loads/        #    2.026 G/sec                       (14.90%)
            79,550      cpu_core/L1-dcache-load-misses/  #    0.13% of all L1-dcache accesses   (14.90%)
           210,135      cpu_atom/LLC-loads/              #    6.946 M/sec                       (54.35%)
            66,439      cpu_core/LLC-loads/              #    2.196 M/sec                       (14.90%)
                 0      cpu_atom/LLC-load-misses/                                               (34.53%)
             1,288      cpu_core/LLC-load-misses/        #    1.94% of all LL-cache accesses    (14.90%)

       8.447642763 seconds time elapsed

       0.010299000 seconds user
       0.020596000 seconds sys
*/