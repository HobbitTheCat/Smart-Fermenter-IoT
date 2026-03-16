#include "FSM/fsm.h"
#include "utils.h"

#include "../h/idleState.h"
#include "../h/runningState.h"
#include "../h/timeEditingState.h"
#include "../h/temperatureState.h"
#include "../h/temperatureEditingState.h"

TemperatureState::TemperatureState() {
    Serial.println("Now on temperature state");
}

StateType TemperatureState::getType() {return StateType::TEMPERATURE;}

void TemperatureState::onShortPress() {
    this->fsm->changeState(new TemperatureEditingState());
}

void TemperatureState::onLongPress() {

}

void TemperatureState::onRotate(int dir) {
    if (! this->fsm->isRunning()) this->fsm->changeState(new IdleState());
    else this->fsm->changeState(new RunningState());
}

void TemperatureState::running() {
    uint32_t now = millis();
    
    if (now - this->lastUpdate >= 200) {
        float newTemp = this->fsm->temperature.getTemperature();
        if (newTemp != this->lastValue) {
            this->fsm->display.drawText(String(newTemp, 2)+"C", 120, 105, 4);
            this->lastValue = newTemp;
        }
        uint32_t remainingTime = this->fsm->timer.getRemainingTime();
        float percent = (float)remainingTime / this->fsm->timer.getInitialDuration();
        uint16_t endAngle = 360 - percent * 360;
        this->fsm->display.drawArc(endAngle);
        this->lastUpdate = now;
    }
}