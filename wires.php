<?php
/**
 * Define the functions and constants from the extension. IDEs can use that to provide code completion.
 */

namespace Carica\Gpio\WiringPi {

    /**
     * GPIO pin numbering, root needed
     */
    const SETUP_GPIO = 0;
    /**
     * GPIO pin numbering, root not needed, pin export using command line tools
     */
    const SETUP_SYSFS = 1;
    /**
     * Wiring Pi pin numbering, root needed
     */
    const SETUP_WIRINGPI = 2;

    /**
     * PIN Mode, digital input
     */
    const INPUT = 0;
    /**
     * PIN Mode, digital output
     */
    const OUTPUT = 1;
    /**
     * PIN Mode, hardware pwm output
     */
    const PWM_OUTPUT = 2;
    /**
     * PIN Mode, gpio clock
     */
    const GPIO_CLOCK = 3;
    /**
     * PIN Mode, software (library) pwm output
     */
    const SOFT_PWM_OUTPUT = 4;
    /**
     * PIN Mode, software (library) tone output
     */
    const SOFT_TONE_OUTPUT = 5;
    /**
     * PIN Mode, hardware tone output (using pwm)
     */
    const PWM_TONE_OUTPUT = 6;

    /**
     * Pin value low (digital)
     */
    const LOW = 0;
    /**
     * Pin value high (digital)
     */
    const HIGH = 1;

    /**
     *  least significant bit first
     */
    const LSBFIRST = 0;

    /**
     *  most significant bit first
     */
    const MSBFIRST = 1;

    /**
     * @param int $mode
     */
    function setup(int $mode = SETUP_GPIO) {};

    /**
     * @param int $pin
     * @param int $mode
     */
    function pinMode(int $pin, int $mode) {};
    /**
     * @param int $pin
     * @param int $mode
     */
    function pinModeAlt(int $pin, int $mode) {};

    /** @noinspection PhpInconsistentReturnPointsInspection
     * @param int $pin
     * @return int
     */
    function digitalRead(int $pin) : int { };
    function digitalWrite(int $pin, int $value) {};

    function pwmWrite(int $pin, int $value) {};

    /** @noinspection PhpInconsistentReturnPointsInspection
     * @param int $pin
     * @return int
     */
    function analogRead(int $pin) : int { };
    function analogWrite(int $pin, int $value) {};

    function shiftOut(int $dataPin, int $clockPin, int $order, int $byte) {};
}

namespace Carica\Gpio\WiringPi\I2C {

    /** 8bit register size */
    const REGISTER_8 = 1;
    /** 16bit register size */
    const REGISTER_16 = 2;

    /** @noinspection PhpInconsistentReturnPointsInspection
    * @param int $deviceAddress
    * @return resource
    */
    function setup(int $deviceAddress) : resource { };
    /** @noinspection PhpInconsistentReturnPointsInspection
     * @param resource $device
     * @return int
     */
    function read(resource $device) : int { };
    /** @noinspection PhpInconsistentReturnPointsInspection
     * @param resource $device
     * @param int $data
     * @return int
     */
    function write(resource $device, int $data) : int { };
    /** @noinspection PhpInconsistentReturnPointsInspection
     * @param resource $device
     * @param int $registerAddress
     * @param $registerSize
     * @return int
     */
    function readRegister(resource $device, int $registerAddress, $registerSize) : int { };
    /** @noinspection PhpInconsistentReturnPointsInspection
     * @param resource $device
     * @param int $registerAddress
     * @param $registerSize
     * @param int $data
     * @return int
     */
    function writeRegister(resource $device, int $registerAddress, $registerSize, int $data) : int { };

}