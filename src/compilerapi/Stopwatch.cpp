#include "Stopwatch.h"

using namespace std;

Stopwatch::Stopwatch()
{
    Reset();
}

void Stopwatch::Start()
{
    if (!isRunning)
    {
        isRunning = true;
        start = chrono::steady_clock::now();
    }
}

void Stopwatch::Stop()
{
    if (isRunning)
    {
        auto stop = chrono::steady_clock::now();
        auto diff = stop - start;
        accum += chrono::duration<double, milli>(diff).count();

        isRunning = false;
    }
}

void Stopwatch::Reset()
{
    accum = 0.0;
    isRunning = false;
}

void Stopwatch::Restart()
{
    Reset();
    Start();
}

bool Stopwatch::IsRunning() const
{
    return isRunning;
}

double Stopwatch::GetElapsed() const
{
    if (isRunning)
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
