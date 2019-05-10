#ifndef TOMATKERNEL_RTC_H
#define TOMATKERNEL_RTC_H

#include <common/stdint.h>
#include <common/stdbool.h>

typedef struct time {
    int seconds;
    int minutes;
    int hours;
    int day;
    int month;
    int year;
} time_t;

void rtc_time(time_t* time);

uint64_t rtc_unixtime();

#endif //TOMATKERNEL_RTC_H
