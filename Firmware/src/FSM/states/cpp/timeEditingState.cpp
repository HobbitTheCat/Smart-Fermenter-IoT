#include "FSM/fsm.h"
#include "utils.h"

#include "../h/idleState.h"
#include "../h/runningState.h"
#include "../h/timeEditingState.h"

TimeEditingState::TimeEditingState() {
    Serial.println("Now on time editing state");
}

StateType TimeEditingState::getType() {return StateType::EDITTIME;}

void TimeEditingState::onShortPress() {
    switch (this->editingState) {
        case HOURS: this->editingState = MINUTES; break;
        case MINUTES: this->editingState = HOURS; break;
    }
}

void TimeEditingState::onLongPress() {
    if (! this->fsm->isRunning()) this->fsm->changeState(new IdleState());
    else this->fsm->changeState(new RunningState());
}

void TimeEditingState::onRotate(int dir) {
    uint32_t additionalAmount;
    switch (this->editingState) {
        case HOURS: additionalAmount = 3600 * 1000; break;
        case MINUTES: additionalAmount = 60 * 1000; break;
    }
    int remainingTime = this->fsm->timer.getRemainingTime()*1000;

    switch (dir) {
        case -1:
            remainingTime -= additionalAmount;
            break;
        case 1:
            remainingTime += additionalAmount;
            break;
    }

    if (remainingTime < 0) remainingTime = 360000000 + remainingTime;
    if (remainingTime >= 360000000) remainingTime -= 360000000;

    this->fsm->timer.setRemainningTime(std::abs(remainingTime));
}

void TimeEditingState::running() {
    uint32_t now = millis();
    if (now - this->lastUpdate >= 200) {
        int remainingTime = this->fsm->timer.getRemainingTime();
        String value;
        switch (this->editingState) {
            case HOURS:
                value = "h" + Utils::getMinutes(remainingTime);
                if (this->lastValue != value){
                    this->fsm->display.drawText(value, 150, 105);
                    this->lastValue = value;
                }
                this->fsm->display.blinkMultipleColor(Utils::getHours(remainingTime), 75, 105, Utils::color(46, 253, 46), TFT_WHITE);
                break;
            case MINUTES:
                value = Utils::getHours(remainingTime);
                if (this->lastValue != value) {
                    this->fsm->display.drawText(value, 75, 105);
                    this->lastValue = value;
                }
                this->fsm->display.blinkMultipleColor(Utils::getMinutes(remainingTime), 165, 105, Utils::color(46, 253, 46), TFT_WHITE);
                break;
        };
        
        float percent = (float)remainingTime / this->fsm->timer.getInitialDuration();
        uint8_t endAngle = 360 - percent * 360;
        this->fsm->display.drawArc(endAngle);
    }
}