#include <TFT_eSPI.h>
#include "utils.h"


#ifndef DISPLAY_H
#define DISPLAY_H

class Display {
    private:
        TFT_eSPI& tft;

        uint16_t arcPosition = 0;
        bool blinkState = true;

        uint16_t bgColor;
        uint16_t arcBgColor;
        uint16_t arcColor;
        
        uint16_t previousTextColor;
        uint8_t previousTextSize;

        String lastBlinkText = "";
        uint32_t lastBlinkUpdate = 0;

        void setTextSize(uint8_t newTextSize) {
            if (newTextSize != this->previousTextSize) {
                this->tft.setTextSize(newTextSize);
                this->previousTextSize = newTextSize;
            }
        }

        void setTextColor(uint16_t newTextColor) {
            if (newTextColor != this->previousTextColor) {
                this->tft.setTextColor(newTextColor, this->bgColor);
                this->previousTextColor = newTextColor;
            }
        }

    public:        
        Display(TFT_eSPI& tft, uint16_t textColor, uint16_t backgroundColor, uint16_t arcColor, uint16_t arcBackgroundColor):
            tft(tft), bgColor(backgroundColor), arcColor(arcColor), arcBgColor(arcBackgroundColor) {
            this->tft.init();
            this->tft.setRotation(2);
            this->tft.setTextSize(5);
            this->previousTextSize = 5;
            this->previousTextColor = textColor;
            this->tft.fillScreen(this->bgColor);
            this->tft.drawSmoothArc(120, 120, 120, 105, 0, 360, this->arcColor, this->bgColor, false);
        }

        void drawArc(uint16_t newPosition) {
            if (newPosition != this->arcPosition){
                if (newPosition == 0) {
                    this->tft.drawSmoothArc(120, 120, 120, 105, 0, 360, this->arcColor, this->bgColor, false);
                }else if (newPosition >= this->arcPosition) {
                    tft.drawSmoothArc(120, 120, 120, 105, 0, newPosition, this->arcBgColor, this->bgColor, false);
                } else {
                    tft.drawSmoothArc(120, 120, 120, 105, newPosition, 0, this->arcColor, this->bgColor, false);
                    tft.drawSmoothArc(120, 120, 120, 105, 0, newPosition, this->arcBgColor, this->bgColor, false);
                }
                this->arcPosition = newPosition;
            }
        }

        void drawText(String text, uint16_t x = 120, uint16_t y = 100, uint8_t textSize = 5, uint16_t textColor = TFT_WHITE,  uint8_t font = 1) {
            this->setTextColor(textColor);
            this->setTextSize(textSize);
            this->tft.drawCentreString(text, x, y, font);
        }

        void eraseAll() {
            this->tft.fillCircle(120, 120, 102, this->bgColor);
        }

        void blinkMultipleColor(String text, uint16_t x, uint16_t y, uint16_t firstTextColor, uint16_t secondTextColor, uint16_t blinkRate = 900, uint8_t textSize = 5, uint8_t font = 1) {
            uint32_t now = millis();

            if (now - this->lastBlinkUpdate >= blinkRate) {
                this->lastBlinkUpdate = now;
                if (this->blinkState) this->setTextColor(firstTextColor);
                else this->setTextColor(secondTextColor);
                this->blinkState = !this->blinkState;
                this->setTextSize(textSize);
                this->tft.drawCentreString(text, x, y, font);
            } else if (text != this->lastBlinkText) {
                if (this->blinkState) this->setTextColor(firstTextColor);
                else this->setTextColor(secondTextColor);
                this->setTextSize(textSize);
                this->tft.drawCentreString(text, x, y, font);
                this->lastBlinkText = text;
            }
        }
};

#endif
