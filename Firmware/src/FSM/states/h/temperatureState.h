#include "FSM/state.h"

#ifndef TEMPERATURESTATE_H
#define TEMPERATURESTATE_H

class TemperatureState : public State{
    private:
        uint32_t lastUpdate = 0;
        float lastValue = 0;
    public:
        TemperatureState();

        void onShortPress() override;
        void onLongPress() override;
        void onRotate(int dir) override;

        void running() override;

        StateType getType() override;
};

#endif