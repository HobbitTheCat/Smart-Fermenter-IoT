#include "FSM/fsm.h"
#include "utils.h"

#include "../h/idleState.h"
#include "../h/runningState.h"
#include "../h/timeEditingState.h"
#include "../h/temperatureState.h"

IdleState::IdleState() {
    Serial.println("Now on idle state");
}

StateType IdleState::getType() {return StateType::IDLE;}

void IdleState::onShortPress() {
    this->fsm->changeState(new TimeEditingState());
}

void IdleState::onLongPress() {
    this->fsm->timer.resume();
    this->fsm->changeState(new RunningState());
}

void IdleState::onRotate(int dir) {
    this->fsm->changeState(new TemperatureState());
}

void IdleState::running() {
    uint32_t now = millis();
    if (now - this->lastUpdate >= 200) {
        this->fsm->display.blinkMultipleColor(Utils::timeFormat(this->fsm->timer.getRemainingTime()), 120, 100,  Utils::color(0, 13, 15), TFT_WHITE, 800);
    }
}