#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "display.h"
#include "utils.h"
#include "encoder.h"
#include "timer.h"
#include "FSM/fsm.h"
#include "FSM/states/h/idleState.h"
#include "tcu.h"
#include "webInterface.h"

TFT_eSPI tft = TFT_eSPI();

Display display(tft, TFT_WHITE, Utils::color(0, 13, 15), Utils::color(253, 157, 46), Utils::color(28, 35, 36));
EncoderReader encoder(34, 35, 32);
Temperature temp(19);
TCU tcu(temp, 21, 10);
Timer timer = Timer(tcu);
FSM fsm(new IdleState(), tft, timer, display, temp, tcu);
WebInterface web(fsm, "SK_WiFiGIGA2E5E_2.4G", "BATCE@2412");

uint32_t lastTimerInfo = 0;

void setup() {
    timer.setRemainningTime(3000000);
    Serial.begin(115200);
    web.begin();
}



void loop() {

    encoder.update();          
    encoder.updateEncoder();

    timer.update();
    temp.update();
    tcu.update();

    
    if (encoder.wasShortPressed()) {
        fsm.handleShortPress();
    }    
    if (encoder.wasLongPressed()) {
        fsm.handleLongPress();
    }
    if (encoder.hasChanged()) {
        fsm.handleRotate(encoder.getDirection());
    }

    fsm.running();
}