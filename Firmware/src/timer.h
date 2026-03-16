#include <Arduino.h>
#include "tcu.h"

#ifndef TIMER_H
#define TIMER_H

class Timer {
    private:
        enum TimerState {
            STOPPED,
            RUNNNING,
            PAUSED
        };

        TimerState state;
        TCU& tcu;

        unsigned long initialDuration;
        unsigned long remainingTime;
        unsigned long lastUpdateTime;
        unsigned long pauseStartTime;
        unsigned long totalPausedTime;

        void (*onCompleteCallback) ();

        unsigned long getCurrentTime() {
            return millis();
        }

        void updateRemainingTime() {
            if (this->state == RUNNNING) {
                unsigned long currentTime = this->getCurrentTime();
                unsigned long elapsed = currentTime - this->lastUpdateTime;

                if (elapsed < this->remainingTime) {
                    this->remainingTime -= elapsed;
                } else {
                    this->remainingTime = 0;
                }

                this->lastUpdateTime = currentTime;
            }
        }

    public:
        Timer (TCU& tcu)
            : state (STOPPED)
            , initialDuration (0)
            , remainingTime (0)
            , lastUpdateTime (0)
            , pauseStartTime (0)
            , totalPausedTime (0)
            , onCompleteCallback(nullptr)
            , tcu(tcu)
        {}

        ~ Timer() {}

        bool start (unsigned long durationSec) {
            this->initialDuration = durationSec * 1000;
            this->remainingTime = durationSec * 1000;
            this->state = RUNNNING;
            this->lastUpdateTime = this->getCurrentTime();
            this->totalPausedTime = 0;

            return true;
        }

        bool start () {
            this->state = RUNNNING;
            this->lastUpdateTime = this->getCurrentTime();
            this->totalPausedTime = 0;

            return true;
        }

        bool pause() {
            if (state != RUNNNING) return false;

            this->updateRemainingTime();
            this->state = PAUSED;
            this->pauseStartTime = this->getCurrentTime();

            return true;
        }

        bool resume() {
            if (state == PAUSED) {
                unsigned long pauseDuration = this->getCurrentTime() - this->pauseStartTime;
                this->totalPausedTime += pauseDuration;

                this->state = RUNNNING;
                this->lastUpdateTime = this->getCurrentTime();

                return true;
            }
            if  (state == STOPPED) {
                if (this->remainingTime == 0) return false;
                this->state = RUNNNING;
                this->lastUpdateTime = this->getCurrentTime();
            }
            return false;
        }

        bool stop() {
            this->state = STOPPED;
            this->remainingTime = 0;
            this->totalPausedTime = 0;

            return true;
        }

        bool setRemainningTime(unsigned long newRemainingTimeMs) {
            Serial.print("Remaining time updated, new time: ");
            Serial.println(newRemainingTimeMs);
            if (this->state == RUNNNING) {
                this->updateRemainingTime();
            }
            this->initialDuration = newRemainingTimeMs;
            this->remainingTime = newRemainingTimeMs;
            this->lastUpdateTime = this->getCurrentTime();
            
            return true;
        }

        unsigned long getRemainingTime() {
            // if (this->state == RUNNNING) {
            //     this->updateRemainingTime();
            // }
            return this->remainingTime / 1000;
        }

        unsigned long getInitialDuration() {
            return this->initialDuration / 1000;
        }

        bool isRunning() {return this->state == RUNNNING; }
        bool isPaused() {return this->state == PAUSED; }
        bool isStopped() {return this->state == STOPPED; }
        bool isCompleted() {return this->state == STOPPED && this->remainingTime == 0; }

        unsigned long getTotalPauseTime() {
            unsigned long total = this->totalPausedTime;
            if (this->state == PAUSED) {
                total += this->getCurrentTime() - this->pauseStartTime;
            }
            return total;
        }

        void setOnCompleteCallback(void (*callback) ()) {
            this->onCompleteCallback = callback;
        }

        void update() {
            if (this->state == RUNNNING) {
                this->updateRemainingTime();

                if(this->remainingTime == 0) {
                    this->state = STOPPED;
                    
                    this->tcu.setTargetTemperature(10);

                    if (this->onCompleteCallback) {
                        this->onCompleteCallback();
                    }
                }
            }
        }

        String getStatusString() {
            String status = "Status: ";
            switch (this->state) {
            case RUNNNING: status += "Running"; break;
            case PAUSED: status += "On pause"; break;
            case STOPPED: status += "Stopped"; break;
            }

            status += "  Remaining time: " + String(this->remainingTime / 1000) + " sec";
            status += " Initial time: " + String(this->initialDuration / 1000) + " sec";
            status += " On pause time: " + String(this->getTotalPauseTime() / 1000) + " sec \n";

            return status;
        }
};

#endif