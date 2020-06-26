#ifndef STOPWATCH_H_
#define STOPWATCH_H_

#include <chrono>
#include <iostream>

//#define TIMING

#ifdef TIMING
#define SW_CREATE(name) Stopwatch name##Stopwatch;
#define SW_START(name) name##Stopwatch.Start();
#define SW_STOP(name) name##Stopwatch.Stop();
#define SW_PRINT(name) std::cout << #name << ": " << name##Stopwatch.GetElapsed() << " ms\n";
#else
#define SW_CREATE(name)
#define SW_START(name)
#define SW_STOP(name)
#define SW_PRINT(name)
#endif

#define SW_BEGIN(name) SW_CREATE(name) SW_START(name)
#define SW_END(name) SW_STOP(name) SW_PRINT(name)

class Stopwatch
{
public:
    Stopwatch();

    void Start();

    void Stop();

    void Reset();

    void Restart();

    bool IsRunning() const;

    double GetElapsed() const;

private:
    std::chrono::time_point<std::chrono::steady_clock> start;
    double accum;
    bool isRunning;
};

#endif // STOPWATCH_H_
