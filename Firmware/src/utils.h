#include<Arduino.h>

#ifndef UTILS_H
#define UTILS_H

class Utils {
    public:
        static uint16_t color(uint8_t r, uint8_t g, uint8_t b) {
            return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }

        static String timeFormat(uint32_t timeSec) {
            uint16_t minutes = timeSec / 60;
            uint8_t secs = timeSec % 60;
            uint8_t hours = minutes/60;
            minutes %= 60;
            
            char buffer[6];
            if (hours == 0) snprintf(buffer, sizeof(buffer), "%02u:%02u", minutes, secs);
            else snprintf(buffer, sizeof(buffer), "%02uh%02u", hours, minutes);

            return buffer;
        }

        static String getHours(uint32_t timeSec) {
            uint16_t minutes = timeSec / 60;
            uint8_t hours = minutes/60;
            char buffer[3];
            snprintf(buffer, sizeof(buffer), "%02u", hours);
            return buffer;
        }

        static String getMinutes(uint32_t timeSec) {
            uint16_t minutes = timeSec/ 60;
            minutes %= 60;
            char buffer[3];
            snprintf(buffer, sizeof(buffer), "%02u", minutes);
            return buffer;
        }
};

#endif