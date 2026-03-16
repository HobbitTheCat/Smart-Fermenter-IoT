#include "FSM/state.h"

#ifndef TIMEEDITINGSTATE_H
#define TIMEEDITINGSTATE_H

class TimeEditingState : public State{
    private:
        uint32_t lastUpdate = 0;

        String lastValue = "";

        enum EditingState {
            MINUTES,
            HOURS,
        };

        EditingState editingState = HOURS;
    public:
        TimeEditingState();

        void onShortPress() override;
        void onLongPress() override;
        void onRotate(int dir) override;

        void running() override;
        StateType getType() override;
};

#endif