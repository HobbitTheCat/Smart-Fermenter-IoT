#include "FSM/state.h"

#ifndef TEMPERATUREEDITINGSTATE_H
#define TEMPERATUREEDITINGSTATE_H

class TemperatureEditingState : public State{
    private:
        uint32_t lastUpdate = 0;
        uint32_t lastValue = 0;
    public:
        TemperatureEditingState();

        void onShortPress() override;
        void onLongPress() override;
        void onRotate(int dir) override;

        void running() override;
        StateType getType() override;
};

#endif