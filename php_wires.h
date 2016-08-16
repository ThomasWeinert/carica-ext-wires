#ifndef PHP_WIRES_H
#define PHP_WIRES_H

#define PHP_WIRES_EXTNAME  "wires"
#define PHP_WIRES_EXTVER   "0.1"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "php.h"

extern zend_module_entry wires_zend_module_entry;
#define phpext_wires_ptr &wires_zend_module_entry

#ifdef ZTS
#include "TSRM.h"
#endif

#if defined(ZTS) && defined(COMPILE_DL_WIRES)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif /* PHP_WIRES_H */
