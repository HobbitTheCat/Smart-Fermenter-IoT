#include "FSM/state.h"

class RunningState;

#ifndef IDLESTATE_H
#define IDLESTATE_H

class IdleState : public State{
    private:
        uint32_t lastUpdate = 0;
        String lastValue = "";
    public:
        IdleState();
        // void onExit() override;

        void onShortPress() override;
        void onLongPress() override;
        void onRotate(int dir) override;

        void running() override;
        StateType getType() override;
};

#endif