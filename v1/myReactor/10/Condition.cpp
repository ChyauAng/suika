#include "Condition.h"

#include <errno.h>
#include <stdint.h>

bool Condition::waitForSeconds(double seconds){
    struct timespec relativetime;
    clock_gettime(CLOCK_MONOTONIC, &relativetime);

    const int64_t KNanoSecondsPerSecond = 1000000000;
    int64_t nanoseconds = static_cast<int64_t>(seconds * KNanoSecondsPerSecond);

    relativetime.tv_sec += static_cast<time_t>((relativetime.tv_nsec + nanoseconds) / KNanoSecondsPerSecond);
    relativetime.tv_nsec = static_cast<time_t>((relativetime.tv_nsec + nanoseconds) % KNanoSecondsPerSecond);

    MutexLock::UnassignGuard ug(mutex_);
    return ETIMEDOUT == pthread_cond_timedwait(&pcond_, mutex_.getMutex(), &relativetime);
}
