#include "PerfCounter.h"

long long PerfCounter::perf_freq = 0;
float PerfCounter::perf_freqf = 0.0f;
PerfCounter PerfCounter::m_Instance{};