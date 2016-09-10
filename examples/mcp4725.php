<?php

use Carica\Gpio\WiringPi\I2C;

$adc = I2C\setup(0x62);

$percent = 0;
while (true) {
    $percent += 0.1;
    if ($percent >= 1.0) {
        $percent = 0;
    }
    $value = (int)floor(4095 * $percent);
    I2C\writeRegister($adc, 0x40, I2C\REGISTER_16, $value << 4);

    printf( "%04d - %016s\n", $value, decbin($value << 4));
    sleep(2);
}