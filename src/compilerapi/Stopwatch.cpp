#include "Stopwatch.h"

using namespace std;

Stopwatch::Stopwatch()
{
    Reset();
}

void Stopwatch::Start()
{
    if (startCount == 0)
    {
        start = chrono::steady_clock::now();
    }
    ++startCount;
}

void Stopwatch::Stop()
{
    if (startCount == 1)
    {
        auto stop = chrono::steady_clock::now();
        auto diff = stop - start;
        accum += chrono::duration<double, milli>(diff).count();

        startCount = 0;
    }
    else if (startCount > 1)
    {
        --startCount;
    }
}

void Stopwatch::Reset()
{
    accum = 0.0;
    startCount = 0;
}

void Stopwatch::Restart()
{
    Reset();
    Start();
}

bool Stopwatch::IsRunning() const
{
    return startCount > 0;
}

double Stopwatch::GetElapsed() const
{
    if (IsRunning())
    {
        auto now = chrono::steady_clock::now();
        auto diff = now - start;
        double extra = chrono::duration<double, milli>(diff).count();
        return accum + extra;
    }
    else
    {
        return accum;
    }
}
