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