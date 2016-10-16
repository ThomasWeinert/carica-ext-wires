#include "php_wires.h"
#include <wiringPiI2C.h>

#define WIRES_NS_I2C "Carica\\Gpio\\WiringPi\\I2C"

#define I2C_REGISTER_8BIT 1
#define I2C_REGISTER_16BIT 2

int le_i2c_device_resource_name;
zend_class_entry *wires_i2c_error_exception;

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

    ZEND_PARSE_PARAMETERS_START(1, 1)
    	Z_PARAM_LONG(deviceAddress)
    ZEND_PARSE_PARAMETERS_END();

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

    ZEND_PARSE_PARAMETERS_START(1, 1)
    	Z_PARAM_RESOURCE(zdevice)
    ZEND_PARSE_PARAMETERS_END();

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
    ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

PHP_FUNCTION(wires_i2c_write) {

    zval *zdevice, *zdata;
    wires_i2c_device *device;
    HashTable *values;

    ZEND_PARSE_PARAMETERS_START(2, 2)
    	Z_PARAM_RESOURCE(zdevice)
    	Z_PARAM_ZVAL(zdata)
    ZEND_PARSE_PARAMETERS_END();

    device = (wires_i2c_device *)zend_fetch_resource(Z_RES_P(zdevice), WIRES_I2C_DEVICE_RESOURCE_NAME, le_i2c_device_resource_name);

    switch (Z_TYPE_P(zdata)) {
    case IS_LONG :
        if (wiringPiI2CWrite(device->descriptor, Z_INTVAL(zdata)) < 0) {
            wires_i2c_throw_error_exception(errno);
        }
        break;
    case IS_ARRAY :
        values = HASH_OF(zdata);
        for (
              zend_hash_internal_pointer_reset(values);
              zend_hash_has_more_elements(values) == SUCCESS;
              zend_hash_move_forward(values)
          ) {
            zval *entry, tmpcopy;
            if ((entry = zend_hash_get_current_data(values)) != NULL) {
                ZVAL_COPY(&tmpcopy, entry);
                convert_to_long(&tmpcopy);
                if (wiringPiI2CWrite(device->descriptor, Z_INTVAL(tmpcopy)) < 0) {
                    wires_i2c_throw_error_exception(errno);
                }
            }
        }
        break;
    default :
        zend_throw_exception_ex(
            spl_ce_InvalidArgumentException, 0, "Argument $data has to be an int or an array of ints."
        );

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

    ZEND_PARSE_PARAMETERS_START(3, 3)
    	Z_PARAM_RESOURCE(zdevice)
    	Z_PARAM_LONG(registerAddress)
    	Z_PARAM_LONG(registerSize)
    ZEND_PARSE_PARAMETERS_END();

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

    ZEND_PARSE_PARAMETERS_START(4, 4)
    	Z_PARAM_RESOURCE(zdevice)
    	Z_PARAM_LONG(registerAddress)
    	Z_PARAM_LONG(registerSize)
    	Z_PARAM_LONG(data)
    ZEND_PARSE_PARAMETERS_END();

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
