#include "FSM/fsm.h"
#include "utils.h"

#include "../h/temperatureEditingState.h"
#include "../h/temperatureState.h"

TemperatureEditingState::TemperatureEditingState() {
    Serial.println("Now on temperature editing");
}

StateType TemperatureEditingState::getType() {return StateType::EDITTEMPERATURE;}

void TemperatureEditingState::onShortPress() {
    this->fsm->changeState(new TemperatureState());
}

void TemperatureEditingState::onLongPress() {
    return;
} 

void TemperatureEditingState::onRotate(int dir) {
    float targetTemperature = this->fsm->tcu.getTargetTemperature();
    switch (dir) {
        case -1:
            targetTemperature -= 0.5;
            break;
        case 1:
            targetTemperature += 0.5;
            break;
    }

    if (targetTemperature < 9.5) targetTemperature = 45;
    if (targetTemperature > 45) targetTemperature = 10;

    this->fsm->tcu.setTargetTemperature(targetTemperature);
}

void TemperatureEditingState::running() {
    uint32_t now = millis();
    if (now - this->lastUpdate >= 200) {
        float targetTemeperature = this->fsm->tcu.getTargetTemperature();
        if (this->lastValue != targetTemeperature) {
            if (targetTemeperature <= 10) {
                this->fsm->display.drawText("  Off  ", 120, 105, 4);
            } else {
                this->fsm->display.drawText(String(targetTemeperature, 2)+"C", 120, 105, 4);
            }
            this->lastValue = targetTemeperature;
        }

        uint32_t remainingTime = this->fsm->timer.getRemainingTime();
        float percent = (float)remainingTime / this->fsm->timer.getInitialDuration();
        uint16_t endAngle = 360 - percent * 360;
        this->fsm->display.drawArc(endAngle);
        this->lastUpdate = now;
    }
}

