#ifndef ENCODERREADER_H
#define ENCODERREADER_H

#include <Arduino.h>

class EncoderReader {
private:
    const unsigned long LONG_PRESS_TIME = 800; // мс
    const unsigned long ENCODER_UPDATE_INTERVAL = 5;

    int _pinA, _pinB, _pinBtn;
    unsigned long _lastEncoderPoll = 0;
    int _encoderPos = 0;
    int _lastState = 0;
    int _lastReported = 0;
    int _direction = 0;

    int _buttonState = HIGH;
    int _lastButtonState = HIGH;
    unsigned long _buttonPressedTime = 0;
    bool _shortPressDetected = false;
    bool _longPressDetected = false;
    bool _longPressReported = false;

    uint32_t lastUnpress = 0;

public:
    EncoderReader(int pinA, int pinB, int pinBtn)
        : _pinA(pinA), _pinB(pinB), _pinBtn(pinBtn) {
        pinMode(_pinA, INPUT);
        pinMode(_pinB, INPUT);
        pinMode(_pinBtn, INPUT_PULLUP);

        _lastState = (digitalRead(_pinA) << 1) | digitalRead(_pinB);
    }

    void update() {
        int reading = digitalRead(_pinBtn);
        uint32_t now = millis();

        if (_lastButtonState == HIGH && reading == LOW) {
            if (now - this->lastUnpress > 100) {
                _buttonPressedTime = now;
                _longPressReported = false;
            } else {
            }
        }
        else if (reading == LOW && !_longPressReported && now - _buttonPressedTime >= LONG_PRESS_TIME) {
            _longPressDetected = true;
            _longPressReported = true;
        }
        else if (_lastButtonState == LOW && reading == HIGH) {
            if (!_longPressReported && now - _buttonPressedTime < LONG_PRESS_TIME) {
                _shortPressDetected = true;
            }
            _buttonPressedTime = 0;
            this->lastUnpress = now;
        }

        _lastButtonState = reading;
    }


    void updateEncoder() {
        unsigned long now = millis();
        if (now - _lastEncoderPoll < ENCODER_UPDATE_INTERVAL) return;
        // Serial.printf("%d\n", now-_lastEncoderPoll);
        _lastEncoderPoll = now;

        int a = digitalRead(_pinA);
        int b = digitalRead(_pinB);
        int state = (a << 1) | b;

        int transition = (_lastState << 2) | state;

        switch (transition) {
            case 0b0001:
            case 0b0111:
            case 0b1110:
            case 0b1000:
                _encoderPos++;
                _direction = 1;
                break;
            case 0b0010:
            case 0b1011:
            case 0b1101:
            case 0b0100:
                _encoderPos--;
                _direction = -1;
                break;
        }

        _lastState = state;
    }

    int getPosition() {
        return _encoderPos / 4;
    }

    bool hasChanged() {
        int current = getPosition();
        if (current != _lastReported) {
            _lastReported = current;
            return true;
        }
        return false;
    }

    bool wasShortPressed() {
        if (_shortPressDetected) {
            _shortPressDetected = false;
            return true;
        }
        return false;
    }

    bool wasLongPressed() {
        if (_longPressDetected) {
            _longPressDetected = false;
            return true;
        }
        return false;
    }

    int getDirection() {
        int dir = _direction;
        _direction = 0;
        return dir;
    }
};

#endif
