#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <OneWire.h>
#include <DallasTemperature.h>

#define TEMP_HISTORY_SIZE 120  // хранить до 120 записей
#define TEMP_HISTORY_INTERVAL_MS 10000

struct TempEntry {
    uint32_t timestamp;
    float temperature;
};

class Temperature {
private:
    OneWire oneWire;
    DallasTemperature sensor;

    TempEntry tempHistory[TEMP_HISTORY_SIZE];
    size_t historyIndex = 0;

    uint32_t lastTempHistoryUpdate = 0;
    uint16_t tempHistoryUpdateIntervalMs = TEMP_HISTORY_INTERVAL_MS;

    float lastTemperature = 0;
    uint32_t lastReadTime = 0;
    uint16_t readIntervalMs = 1000;

public:
    Temperature(uint8_t pin) : oneWire(pin), sensor(&oneWire) {
        sensor.begin();
        sensor.requestTemperatures();
    }

    void update() {
        uint32_t now = millis();
        if (now - lastReadTime >= readIntervalMs) {
            sensor.requestTemperatures();
            lastTemperature = sensor.getTempCByIndex(0);

            if (now - lastTempHistoryUpdate >= tempHistoryUpdateIntervalMs) {
                uint32_t seconds = now / 1000;
                tempHistory[historyIndex] = {seconds, lastTemperature};
                historyIndex = (historyIndex + 1) % TEMP_HISTORY_SIZE;
                lastTempHistoryUpdate = now;
            }

            lastReadTime = now;
        }
    }

    float getTemperature() const {
        return lastTemperature;
    }

    size_t getHistoryCount() const {
        return TEMP_HISTORY_SIZE;
    }

    const TempEntry& getHistoryEntry(size_t i) const {
        return tempHistory[i];
    }
};

#endif
