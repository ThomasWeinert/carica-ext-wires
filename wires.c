#include "php_wires.h"
#include "ext/standard/info.h"
#include "ext/spl/spl_exceptions.h"
#include <wiringPi.h>
#include <wiringShift.h>

#define WIRES_NS "Carica\\Gpio\\WiringPi"

#define SETUP_GPIO 0
#define SETUP_SYSFS 1
#define SETUP_WIRINGPI 2

ZEND_BEGIN_ARG_INFO(ArgInfo_SetupMode, 0)
    ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(ArgInfo_PinReturnValue, IS_LONG, NULL, 0)
    ZEND_ARG_TYPE_INFO(0, pin, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(ArgInfo_PinAndMode, 0)
    ZEND_ARG_TYPE_INFO(0, pin, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(ArgInfo_PinAndValue, 0)
    ZEND_ARG_TYPE_INFO(0, pin, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(ArgInfo_wires_shiftOut, 0)
    ZEND_ARG_TYPE_INFO(0, dataPin, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, clockPin, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, order, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, byte, IS_LONG, 0)
ZEND_END_ARG_INFO()

PHP_FUNCTION(wires_setup)
{
    zend_long mode = SETUP_GPIO;
    zend_long error = 0;
    char message[128];

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &mode) == FAILURE) {
        RETURN_NULL();
    }

    putenv("WIRINGPI_CODES=1");
    if (mode == SETUP_GPIO) {
        error = wiringPiSetupGpio();
    } else if (mode == SETUP_SYSFS) {
        error = wiringPiSetupSys();
    } else if (mode == SETUP_WIRINGPI) {
        error = wiringPiSetup();
    } else {
        zend_throw_exception_ex(
            spl_ce_OutOfRangeException, 0, "Use Carica\\Gpio\\WiringPi::SETUP_* constant."
        );
    }
    if (error != 0) {
        snprintf(message, 128, "WiringPi setup returned error code %d.", error);
        zend_throw_exception_ex(
            spl_ce_UnexpectedValueException, 0, message
        );
    }
}

PHP_FUNCTION(wires_pinMode)
{
    zend_long pin;
    zend_long mode;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &pin, &mode) == FAILURE) {
        RETURN_NULL();
    }
    pinMode(pin, mode);
}

PHP_FUNCTION(wires_pinModeAlt)
{
    zend_long pin;
    zend_long mode;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &pin, &mode) == FAILURE) {
        RETURN_NULL();
    }
    pinModeAlt(pin, mode);
}

PHP_FUNCTION(wires_pullUpDnControl)
{
    zend_long pin;
    zend_long pud;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &pin, &pud) == FAILURE) {
        RETURN_NULL();
    }
    pullUpDnControl(pin, pud);
}

PHP_FUNCTION(wires_digitalRead)
{
    zend_long pin;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &pin) == FAILURE) {
        RETURN_NULL();
    }
    RETURN_LONG(digitalRead(pin));
}

PHP_FUNCTION(wires_digitalWrite)
{
    zend_long pin;
    zend_long value;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &pin, &value) == FAILURE) {
        RETURN_NULL();
    }
    digitalWrite(pin, (value == HIGH) ? HIGH : LOW);
}

PHP_FUNCTION(wires_pwmWrite)
{
    zend_long pin;
    zend_long value;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &pin, &value) == FAILURE) {
        RETURN_NULL();
    }
    pwmWrite(pin, value);
}

PHP_FUNCTION(wires_analogRead)
{
    zend_long pin;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &pin) == FAILURE) {
        RETURN_NULL();
    }
    RETURN_LONG(analogRead(pin));
}

PHP_FUNCTION(wires_analogWrite)
{
    zend_long pin;
    zend_long value;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &pin, &value) == FAILURE) {
        RETURN_NULL();
    }
    analogWrite(pin, value);
}

PHP_FUNCTION(wires_shiftOut)
{
    zend_long dataPin, clockPin, order, value;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llll", &dataPin, &clockPin, &order, &value) == FAILURE) {
        RETURN_NULL();
    }
    shiftOut(dataPin, clockPin, order, value);
}

PHP_MINFO_FUNCTION(wires)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "Wires Support", "enabled");
    php_info_print_table_row(2, "Extension Version", PHP_WIRES_EXTVER);
    php_info_print_table_end();
}

PHP_MINIT_FUNCTION(wires)
{
    REGISTER_NS_LONG_CONSTANT(WIRES_NS, "SETUP_GPIO", SETUP_GPIO, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(WIRES_NS, "SETUP_SYSFS", SETUP_SYSFS, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(WIRES_NS, "SETUP_WIRINGPI", SETUP_WIRINGPI, CONST_CS | CONST_PERSISTENT);

    REGISTER_NS_LONG_CONSTANT(WIRES_NS, "INPUT", INPUT, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(WIRES_NS, "OUTPUT", OUTPUT, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(WIRES_NS, "PWM_OUTPUT", PWM_OUTPUT, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(WIRES_NS, "GPIO_CLOCK", GPIO_CLOCK, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(WIRES_NS, "SOFT_PWM_OUTPUT", SOFT_PWM_OUTPUT, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(WIRES_NS, "SOFT_TONE_OUTPUT", SOFT_TONE_OUTPUT, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(WIRES_NS, "PWM_TONE_OUTPUT", PWM_TONE_OUTPUT, CONST_CS | CONST_PERSISTENT);

    REGISTER_NS_LONG_CONSTANT(WIRES_NS, "LOW", LOW, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(WIRES_NS, "HIGH", HIGH, CONST_CS | CONST_PERSISTENT);

    REGISTER_NS_LONG_CONSTANT(WIRES_NS, "LSBFIRST", LSBFIRST, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(WIRES_NS, "MSBFIRST", MSBFIRST, CONST_CS | CONST_PERSISTENT);
}

const zend_function_entry php_wires_functions[] = {
    ZEND_NS_NAMED_FE(WIRES_NS, setup, ZEND_FN(wires_setup), ArgInfo_SetupMode)
    ZEND_NS_NAMED_FE(WIRES_NS, pinMode, ZEND_FN(wires_pinMode), ArgInfo_PinAndMode)
    ZEND_NS_NAMED_FE(WIRES_NS, pinModeAlt, ZEND_FN(wires_pinModeAlt), ArgInfo_PinAndMode)
    ZEND_NS_NAMED_FE(WIRES_NS, digitalRead, ZEND_FN(wires_digitalRead), ArgInfo_PinReturnValue)
    ZEND_NS_NAMED_FE(WIRES_NS, digitalWrite, ZEND_FN(wires_digitalWrite), ArgInfo_PinAndValue)
    ZEND_NS_NAMED_FE(WIRES_NS, pwmWrite, ZEND_FN(wires_pwmWrite), ArgInfo_PinAndValue)
    ZEND_NS_NAMED_FE(WIRES_NS, analogRead, ZEND_FN(wires_analogRead), ArgInfo_PinReturnValue)
    ZEND_NS_NAMED_FE(WIRES_NS, analogWrite, ZEND_FN(wires_analogWrite), ArgInfo_PinAndValue)
    ZEND_NS_NAMED_FE(WIRES_NS, shiftOut, ZEND_FN(wires_shiftOut), ArgInfo_wires_shiftOut)
    PHP_FE_END
};

zend_module_entry wires_module_entry = {
  STANDARD_MODULE_HEADER,
  PHP_WIRES_EXTNAME,
  php_wires_functions, /* Functions */
  PHP_MINIT(wires), /* MINIT */
  NULL, /* MSHUTDOWN */
  NULL, /* RINIT */
  NULL, /* RSHUTDOWN */
  PHP_MINFO(wires), /* MINFO */
  PHP_WIRES_EXTVER,
  STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_WIRES
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(wires)
#endif
