#include "FSM/fsm.h"
#include "utils.h"

#include "../h/idleState.h"
#include "../h/runningState.h"
#include "../h/timeEditingState.h"
#include "../h/temperatureState.h"

RunningState::RunningState() {
    Serial.println("Now on running state");
}

StateType RunningState::getType() {return StateType::RUNNING;}

void RunningState::onShortPress() {
    this->fsm->timer.pause();
    this->fsm->changeState(new IdleState());
}

void RunningState::onLongPress() {
    this->fsm->changeState(new TimeEditingState());
}

void RunningState::onRotate(int dir) {
    this->fsm->changeState(new TemperatureState());
}

void RunningState::running() {
    uint32_t now = millis();
    if (now - this->lastUpdate >= 200) {
        uint32_t remainingTime = this->fsm->timer.getRemainingTime();
        String timeFormated = Utils::timeFormat(remainingTime);

        if (timeFormated !=  this->lastValue){
            this->fsm->display.drawText(timeFormated);
            this->lastValue = timeFormated;
        }
        
        float percent = (float)remainingTime / this->fsm->timer.getInitialDuration();
        uint16_t endAngle = 360 - percent * 360;
        this->fsm->display.drawArc(endAngle);
    }
}