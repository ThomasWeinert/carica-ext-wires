<?php

namespace Carica\Gpio\WiringPi {

    const SETUP_GPIO = 0;
    const SETUP_SYSFS = 1;
    const SETUP_GPIO = 2;

    const INPUT = 0;
    const OUTPUT = 1;
    const PWM_OUTPUT = 2;
    const GPIO_CLOCK = 3;
    const SOFT_PWM_OUTPUT = 4;
    const SOFT_TONE_OUTPUT = 5;
    const PWM_TONE_OUTPUT = 6;

    const LOW = 0;
    const HIGH = 1;

    const LSBFIRST = 0;
    const MSBFIRST = 1;

    function setup(int $mode = SETUP_GPIO) {};

    function pinMode(int $pin, int $mode) {};
    function pinModeAlt(int $pin, int $mode) {};

    function digitalRead(int $pin) : int {};
    function digitalWrite(int $pin, int $value) {};

    function pwmWrite(int $pin, int $value) {};

    function analogRead(int $pin) : int {};
    function analogWrite(int $pin, int $value) {};

    function shiftOut(int $dataPin, int $clockPin, int $order, int $byte) {};
}