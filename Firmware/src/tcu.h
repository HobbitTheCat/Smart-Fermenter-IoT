#include "temperature.h"

#ifndef TEMPERATURE_CONTROL_UNIT_H
#define TEMPERATURE_CONTROL_UNIT_H

class TCU {
    private:
        Temperature &temp;
        uint8_t relayPin;

        // float Kp = 2.0;
        // float Ki = 0.1;
        // float Kd = 0.5;

        float targetTemp;
        float hysteresis = 0.25;
        // float integral = 0.0; 
        // float lastError = 0.0;

        // unsigned long lastPIDTime = 0;
        // unsigned long windowStartTime = 0; 

        // const unsigned long pidIntervalMs = 30000;
        // const unsigned long minOnDurationMs = 5000;
        // const unsigned long relayControlWindowMs = 30000; 

        // unsigned long relayOnDuration = 0; 
        bool relayState = false;

    public:
        TCU(Temperature &sensor, uint8_t relayPin, float targetTemp) 
        : temp(sensor), relayPin(relayPin){
            pinMode(this->relayPin, OUTPUT);
            digitalWrite(this->relayPin, LOW);
            // this->lastPIDTime = millis();
            // this->windowStartTime = millis();
            this->setTargetTemperature(targetTemp); 
        }

        // void update() {
        //     unsigned long now = millis();

        //     if (now - this->lastPIDTime >= this->pidIntervalMs) {

        //         float currentTemp = this->temp.getTemperature();
        //         float error = this->targetTemp - currentTemp;

        //         float dt = (now - this->lastPIDTime) / 1000.0;

        //         this->integral += error * dt;
                
        //         this->integral = constrain(this->integral, -100, 100);
                
        //         float derivative = (error - this->lastError) / dt;

        //         float output = this->Kp * error + this->Ki * this->integral + this->Kd * derivative;

        //         float outputPercent = constrain(output, 0, 100) / 100.0;
        //         this->relayOnDuration = (unsigned long)(outputPercent * this->relayControlWindowMs);

        //         this->lastError = error;
        //         this->lastPIDTime = now;
                
        //         // Serial.print("PID Output: ");
        //         // Serial.print(outputPercent * 100);
        //         // Serial.print("%, Relay ON duration: ");
        //         // Serial.print(this->relayOnDuration);
        //         // Serial.print("ms, Target: ");
        //         // Serial.print(this->targetTemp);
        //         // Serial.print("C Error: ");
        //         // Serial.println(error);
        //     }

        //     unsigned long windowElapsed = now - this->windowStartTime;
            
        //     if (windowElapsed >= this->relayControlWindowMs) {
        //         this->windowStartTime = now;
        //         windowElapsed = 0;
        //     }

        //     bool shouldBeOn = (windowElapsed < this->relayOnDuration);
            
        //     if (shouldBeOn && !this->relayState) {
        //         digitalWrite(this->relayPin, HIGH);
        //         this->relayState = true;
        //         Serial.println("Включили реле");
        //     } else if (!shouldBeOn && this->relayState) {
        //         digitalWrite(this->relayPin, LOW);
        //         this->relayState = false;
        //         Serial.println("Выключили реле");
        //     }
        // }

        void update() {
            float currentTemp = this->temp.getTemperature();

            if (!this->relayState && currentTemp < targetTemp - hysteresis) {
                digitalWrite(this->relayPin, HIGH);
                this->relayState = true;
                Serial.println("Включили реле");
            } else if (this->relayState && currentTemp > targetTemp + hysteresis) {
                digitalWrite(this->relayPin, LOW);
                this->relayState = false;
                Serial.println("Выключили реле");
            }
        }


        void setTargetTemperature(float temp) { 
            this->targetTemp = temp;
            // this->integral = 0.0;
        }

        float getTargetTemperature() const {
            return this->targetTemp;
        }

        // float getCurrentOutput() const {
        //     return (float)this->relayOnDuration / this->relayControlWindowMs * 100.0;
        // }
            
        // void setPIDParams(float kp, float ki, float kd) {
        //     this->Kp = kp;
        //     this->Ki = ki;
        //     this->Kd = kd;
        // }
};

#endif