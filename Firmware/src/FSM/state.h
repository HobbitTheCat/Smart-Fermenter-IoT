#include <Arduino.h>
#include "states/stateTypes.h"

#pragma once

class FSM;

#ifndef STATE_H
#define STATE_H

class State {
    protected:
        FSM* fsm;
    
    public:
        virtual void onShortPress() {};
        virtual void onLongPress() {};
        virtual void onRotate(int dir) {};
        
        virtual void running() {};
        
        virtual StateType getType();
        void setFSM(FSM* fsmPtr) {this->fsm = fsmPtr;}
        virtual ~State() {};
};

#endif