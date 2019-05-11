#include "rtc.h"
#include "portio.h"

#define RTC_REG_ADDRESS     0x70
#define RTC_REG_DATA        0x71

#define RTC_ADDR_SECONDS        (uint8_t)0x0u
#define RTC_ADDR_MINUTES        (uint8_t)0x2u
#define RTC_ADDR_HOURS          (uint8_t)0x4u
#define RTC_ADDR_WEEKDAY        (uint8_t)0x6u
#define RTC_ADDR_DAY_OF_MONTH   (uint8_t)0x7u
#define RTC_ADDR_MONTH          (uint8_t)0x8u
#define RTC_ADDR_YEAR           (uint8_t)0x9u

#define RTC_ADDR_STATUS_A       (uint8_t)0xau
#define RTC_ADDR_STATUS_B       (uint8_t)0xbu

static uint16_t rtc_read(uint8_t addr) {
    outb(RTC_REG_ADDRESS, addr);
    return inb(RTC_REG_DATA);
}

void rtc_time(time_t* time) {
    time->seconds = rtc_read(RTC_ADDR_SECONDS);
    time->minutes = rtc_read(RTC_ADDR_MINUTES);
    time->hours = rtc_read(RTC_ADDR_HOURS);
    time->day = rtc_read(RTC_ADDR_DAY_OF_MONTH);
    time->month = rtc_read(RTC_ADDR_MONTH);
    time->year = rtc_read(RTC_ADDR_YEAR);

    // check if encoded in bcd
    if((rtc_read(RTC_ADDR_STATUS_B) & 0x4) == 0) {
        time->seconds = (time->seconds & 0xf) + ((time->seconds / 16) * 10);
        time->minutes = (time->minutes & 0xf) + ((time->minutes / 16) * 10);
        time->hours = ((time->hours & 0xf) + (((time->hours & 0x70) / 16) * 10)) | (time->hours & 0x80);
        time->day = (time->day & 0xf) + ((time->day / 16) * 10);
        time->month = (time->month & 0xf) + ((time->month / 16) * 10);
        time->year = (time->year & 0xf) + ((time->year / 16) * 10);
    }
}

uint64_t rtc_unixtime() {
    time_t time;
    rtc_time(&time);
    return (uint64_t) ( time.seconds +
                        time.minutes * 60 +
                        time.hours * 3600 +
                        time.day * 86400 +
                        time.month * 2629743 +
                        time.year * 31556926);
}

