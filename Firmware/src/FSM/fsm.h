
#include "FSM/state.h"
#include "display.h"
#include "timer.h"
#include "temperature.h"
#include "tcu.h"
#include <TFT_eSPI.h>

#ifndef FSM_H
#define FSM_H

class FSM {
    private:
        State* current;

        bool on = false;
    public:
        TFT_eSPI& tft;
        Timer& timer;
        Display& display;
        Temperature& temperature;
        TCU& tcu;
        


        FSM(State* initial, TFT_eSPI& tft, Timer& timer, Display& display, Temperature& temp, TCU& tcu)
        : current(nullptr), tft(tft), timer(timer), display(display), temperature(temp), tcu(tcu) {
            changeState(initial);
        }

        ~FSM() {
            if (current) {
                delete current;
                current = nullptr;
            }
        }

        void changeState(State* newState) {
            this->display.eraseAll();
            String state;
            switch (newState->getType()) {
                case StateType::IDLE : state = "stopped"; this->on = false; break;
                case StateType::RUNNING : state = "running"; this->on = true; break;
                case StateType::EDITTIME : state = "time edit"; break;
                case StateType::TEMPERATURE: state = "temp"; break;
                case StateType::EDITTEMPERATURE: state = "set temp"; break;
                default: state = "went wrong"; break;
            }

            this->display.drawText(state, 120, 50, 2);

            this->current = newState;
            this->current->setFSM(this);
        }

        bool isRunning() {
            return this->on;
        }

        void handleShortPress() {current->onShortPress(); }
        void handleLongPress() {current->onLongPress(); }
        void handleRotate(int d) {current->onRotate(d);}

        void running() {this->current->running();}
};


#endif