<?php
declare(strict_types=1);

use Carica\Gpio\WiringPi;

WiringPi\setup(WiringPi\SETUP_GPIO);

$pin = 26;
WiringPi\pinMode($pin, WiringPi\OUTPUT);

$on = false;
while (TRUE) {
    echo ($on = !$on) ? "on\n" : "off\n";
    WiringPi\digitalWrite($pin, $on ? WiringPi\HIGH : WiringPi\LOW);
    sleep(1);
}