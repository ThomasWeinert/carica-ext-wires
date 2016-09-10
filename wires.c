#include "php_wires.h"
#include "ext/standard/info.h"
#include <zend_exceptions.h>
#include "ext/spl/spl_exceptions.h"
#include <wiringPi.h>
#include <wiringShift.h>
#include <wiringPiI2C.h>

#define WIRES_NS "Carica\\Gpio\\WiringPi"
#define WIRES_NS_I2C "Carica\\Gpio\\WiringPi\\I2C"

#define SETUP_GPIO 0
#define SETUP_SYSFS 1
#define SETUP_WIRINGPI 2

#define I2C_REGISTER_8BIT 1
#define I2C_REGISTER_16BIT 2

int le_i2c_device_resource_name;
zend_class_entry *wires_i2c_error_exception;

/* Generic GPIO (Pin Handling) */
/* =========================== */

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

/* Shiftout */
/* ======== */

ZEND_BEGIN_ARG_INFO(ArgInfo_wires_shiftOut, 0)
    ZEND_ARG_TYPE_INFO(0, dataPin, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, clockPin, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, order, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, byte, IS_LONG, 0)
ZEND_END_ARG_INFO()

PHP_FUNCTION(wires_shiftOut)
{
    zend_long dataPin, clockPin, order, value;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llll", &dataPin, &clockPin, &order, &value) == FAILURE) {
        RETURN_NULL();
    }
    shiftOut(dataPin, clockPin, order, value);
}

/* I2C */
/* === */

static void wires_i2c_device_dtor(zend_resource *rsrc)
{
    wires_i2c_device *device = (wires_i2c_device*)rsrc->ptr;

    if (device) {
        //fclose((FILE*)device->descriptor);
        efree(device);
    }
}

void wires_init_i2c_error_exception(TSRMLS_D) {
    zend_class_entry e;

    INIT_NS_CLASS_ENTRY(e, WIRES_NS_I2C, "ErrorException", NULL);
    wires_i2c_error_exception = zend_register_internal_class_ex(&e, (zend_class_entry*)zend_exception_get_default(TSRMLS_C));
}

static void wires_i2c_throw_error_exception(zend_long error) {

    char message[128];

    snprintf(message, 128, "WiringPi i2c error: %d.", error);
    zend_throw_exception_ex(
        wires_i2c_error_exception, error, message
    );
}

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(ArgInfo_wires_i2c_setup, IS_RESOURCE, NULL, 0)
    ZEND_ARG_TYPE_INFO(0, deviceAddress, IS_LONG, 0)
ZEND_END_ARG_INFO()

PHP_FUNCTION(wires_i2c_setup) {

    zend_long deviceAddress;
    wires_i2c_device *device;
    zend_long descriptor;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &deviceAddress) == FAILURE) {
        RETURN_NULL();
    }

    descriptor = wiringPiI2CSetup(deviceAddress);
    if (descriptor < 0) {
        wires_i2c_throw_error_exception(errno);
    } else {
        device = emalloc(sizeof(wires_i2c_device));
        device->descriptor = descriptor;
        RETURN_RES(zend_register_resource(device, le_i2c_device_resource_name));
    }

}

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(ArgInfo_wires_i2c_read, IS_LONG, NULL, 0)
    ZEND_ARG_TYPE_INFO(0, device, IS_RESOURCE, 0)
ZEND_END_ARG_INFO()

PHP_FUNCTION(wires_i2c_read) {

    wires_i2c_device *device;
    zval *zdevice;
    zend_long data;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zdevice) == FAILURE) {
        RETURN_NULL();
    }
    device = (wires_i2c_device *)zend_fetch_resource(Z_RES_P(zdevice), WIRES_I2C_DEVICE_RESOURCE_NAME, le_i2c_device_resource_name);

    data = wiringPiI2CRead(device->descriptor);
    if (data > 0) {
      RETURN_LONG(data);
    } else {
      wires_i2c_throw_error_exception(errno);
    }
}

ZEND_BEGIN_ARG_INFO(ArgInfo_wires_i2c_write, 0)
    ZEND_ARG_TYPE_INFO(0, device, IS_RESOURCE, 0)
    ZEND_ARG_TYPE_INFO(0, data, IS_LONG, 0)
ZEND_END_ARG_INFO()

PHP_FUNCTION(wires_i2c_write) {

    wires_i2c_device *device;
    zval *zdevice;
    zend_long data;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zdevice, &data) == FAILURE) {
        RETURN_NULL();
    }
    device = (wires_i2c_device *)zend_fetch_resource(Z_RES_P(zdevice), WIRES_I2C_DEVICE_RESOURCE_NAME, le_i2c_device_resource_name);

    if (wiringPiI2CWrite(device->descriptor, data) < 0) {
        wires_i2c_throw_error_exception(errno);
    }
}

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(ArgInfo_wires_i2c_readRegister, IS_LONG, NULL, 0)
    ZEND_ARG_TYPE_INFO(0, device, IS_RESOURCE, 0)
    ZEND_ARG_TYPE_INFO(0, registerAddress, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, registerSize, IS_LONG, 0)
ZEND_END_ARG_INFO()

PHP_FUNCTION(wires_i2c_readRegister) {

    wires_i2c_device *device;
    zval *zdevice;
    zend_long registerAddress, registerSize, data;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rll", &zdevice, &registerAddress, &registerSize) == FAILURE) {
        RETURN_NULL();
    }
    device = (wires_i2c_device *)zend_fetch_resource(Z_RES_P(zdevice), WIRES_I2C_DEVICE_RESOURCE_NAME, le_i2c_device_resource_name);

    switch (registerSize) {
    case I2C_REGISTER_8BIT :
      data = wiringPiI2CReadReg8(device->descriptor, registerAddress);
      break;
    case I2C_REGISTER_16BIT :
      data = wiringPiI2CReadReg16(device->descriptor, registerAddress);
      break;
    default :
        zend_throw_exception_ex(
            spl_ce_OutOfRangeException, 0, "Use Carica\\Gpio\\WiringPi\\I2C::REGISTER_* constant."
        );
        return;
    }
    if (data < 0) {
        wires_i2c_throw_error_exception(errno);
    } else {
        RETURN_LONG(data);
    }
}

ZEND_BEGIN_ARG_INFO(ArgInfo_wires_i2c_writeRegister, 0)
    ZEND_ARG_TYPE_INFO(0, device, IS_RESOURCE, 0)
    ZEND_ARG_TYPE_INFO(0, registerAddress, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, registerSize, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, data, IS_LONG, 0)
ZEND_END_ARG_INFO()

PHP_FUNCTION(wires_i2c_writeRegister) {

    wires_i2c_device *device;
    zval *zdevice;
    zend_long registerAddress, registerSize, data, result;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlll", &zdevice, &registerAddress, &registerSize, &data) == FAILURE) {
        RETURN_NULL();
    }
    device = (wires_i2c_device *)zend_fetch_resource(Z_RES_P(zdevice), WIRES_I2C_DEVICE_RESOURCE_NAME, le_i2c_device_resource_name);

    switch (registerSize) {
    case I2C_REGISTER_8BIT :
        result = wiringPiI2CWriteReg8(device->descriptor, registerAddress, data);
        break;
    case I2C_REGISTER_16BIT :
        result = wiringPiI2CWriteReg16(device->descriptor, registerAddress, data);
        break;
    default :
        zend_throw_exception_ex(
            spl_ce_OutOfRangeException, 0, "Use Carica\\Gpio\\WiringPi\\I2C::REGISTER_* constant."
        );
        return;
    }
    if (result < 0) {
        wires_i2c_throw_error_exception(errno);
    }

}

/* PHP Extension Hooks */
/* =================== */

PHP_MINFO_FUNCTION(wires)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "Wires Support", "enabled");
    php_info_print_table_row(2, "Extension Version", PHP_WIRES_EXTVER);
    php_info_print_table_end();
}

PHP_MINIT_FUNCTION(wires)
{
    /* GPIO */
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

    /* ShiftOut */
    REGISTER_NS_LONG_CONSTANT(WIRES_NS, "LSBFIRST", LSBFIRST, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(WIRES_NS, "MSBFIRST", MSBFIRST, CONST_CS | CONST_PERSISTENT);

    /* I2C */
    REGISTER_NS_LONG_CONSTANT(WIRES_NS_I2C, "REGISTER_8", I2C_REGISTER_8BIT, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(WIRES_NS_I2C, "REGISTER_16", I2C_REGISTER_16BIT, CONST_CS | CONST_PERSISTENT);
    /* I2C resource */
    le_i2c_device_resource_name = zend_register_list_destructors_ex(wires_i2c_device_dtor, NULL, WIRES_I2C_DEVICE_RESOURCE_NAME, module_number);
    /* i2C error exception */
    wires_init_i2c_error_exception(TSRMLS_C);
}

const zend_function_entry php_wires_functions[] = {
    /* GPIO */
    ZEND_NS_NAMED_FE(WIRES_NS, setup, ZEND_FN(wires_setup), ArgInfo_SetupMode)
    ZEND_NS_NAMED_FE(WIRES_NS, pinMode, ZEND_FN(wires_pinMode), ArgInfo_PinAndMode)
    ZEND_NS_NAMED_FE(WIRES_NS, pinModeAlt, ZEND_FN(wires_pinModeAlt), ArgInfo_PinAndMode)
    ZEND_NS_NAMED_FE(WIRES_NS, digitalRead, ZEND_FN(wires_digitalRead), ArgInfo_PinReturnValue)
    ZEND_NS_NAMED_FE(WIRES_NS, digitalWrite, ZEND_FN(wires_digitalWrite), ArgInfo_PinAndValue)
    ZEND_NS_NAMED_FE(WIRES_NS, pwmWrite, ZEND_FN(wires_pwmWrite), ArgInfo_PinAndValue)
    ZEND_NS_NAMED_FE(WIRES_NS, analogRead, ZEND_FN(wires_analogRead), ArgInfo_PinReturnValue)
    ZEND_NS_NAMED_FE(WIRES_NS, analogWrite, ZEND_FN(wires_analogWrite), ArgInfo_PinAndValue)
    /* ShiftOut */
    ZEND_NS_NAMED_FE(WIRES_NS, shiftOut, ZEND_FN(wires_shiftOut), ArgInfo_wires_shiftOut)
    /* I2C */
    ZEND_NS_NAMED_FE(WIRES_NS_I2C, setup, ZEND_FN(wires_i2c_setup), ArgInfo_wires_i2c_setup)
    ZEND_NS_NAMED_FE(WIRES_NS_I2C, read, ZEND_FN(wires_i2c_read), ArgInfo_wires_i2c_read)
    ZEND_NS_NAMED_FE(WIRES_NS_I2C, write, ZEND_FN(wires_i2c_write), ArgInfo_wires_i2c_write)
    ZEND_NS_NAMED_FE(WIRES_NS_I2C, readRegister, ZEND_FN(wires_i2c_readRegister), ArgInfo_wires_i2c_readRegister)
    ZEND_NS_NAMED_FE(WIRES_NS_I2C, writeRegister, ZEND_FN(wires_i2c_writeRegister), ArgInfo_wires_i2c_writeRegister)
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
