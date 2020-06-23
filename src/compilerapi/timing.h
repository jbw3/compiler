#ifndef TIMING_H_
#define TIMING_H_

#include <chrono>
#include <iostream>

//#define TIMING

#ifdef TIMING
#define START_TIMER(name) \
    auto name##Start = std::chrono::steady_clock::now();
#else
#define START_TIMER(name)
#endif

#ifdef TIMING
#define END_TIMER(name) \
    auto name##End = std::chrono::steady_clock::now(); \
    auto name##Diff = name##End - name##Start; \
    std::cout << #name << ": " << std::chrono::duration<double, milli>(name##Diff).count() << " ms\n";
#else
#define END_TIMER(name)
#endif

#endif // TIMING_H_
