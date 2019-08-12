#ifndef TOMATKERNEL_RTC_H
#define TOMATKERNEL_RTC_H

#include <error.h>
#include <stdint.h>

typedef struct time {
    int seconds;
    int minutes;
    int hours;
    int day;
    int month;
    int year;
} time_t;

/**
 * Get the rtc date and time
 *
 * @param time  [OUT] The time struct we are going to populate with the date and time
 *
 * @return NO_ERROR: Success
 */
error_t rtc_time(time_t* time);

/**
 * Will return the time in seconds since 1970
 *
 * @remark
 * This uses a simple formula to do so and is not that accurate
 *
 * @param time  [OUT] The output time in seconds
 *
 * @return NO_ERROR: Success
 */
error_t rtc_unixtime(uint64_t* time);

#endif //TOMATKERNEL_RTC_H
