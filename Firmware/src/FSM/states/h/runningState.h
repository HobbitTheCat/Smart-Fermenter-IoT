#include "FSM/state.h"

class IdleState;

#ifndef RUNNINGSTATE_H
#define RUNNINGSTATE_H

class RunningState : public State{
    private:
        uint32_t lastUpdate = 0;

        String lastValue = "";
    public:
        RunningState();

        void onShortPress() override;
        void onLongPress() override;
        void onRotate(int dir) override;

        void running() override;
        StateType getType() override;
};

#endif