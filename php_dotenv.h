#ifndef PHP_DOTENV_H
#define PHP_DOTENV_H

#include <main/php.h>

#define PHP_DOTENV_EXTNAME     "dotenv"
#define PHP_DOTENV_EXTVER      "0.3"

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#define DOTENV_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(dotenv, v)

#if defined(__GNUC__) && __GNUC__ >= 4
#	define DOTENV_VISIBILITY_HIDDEN __attribute__((visibility("hidden")))
#else
#	define DOTENV_VISIBILITY_HIDDEN
#endif

#define phpext_dotenv_ptr  &dotenv_module_entry

ZEND_BEGIN_MODULE_GLOBALS(dotenv)
	HashTable entries;
	char* filename;
	zend_bool populate_env;
ZEND_END_MODULE_GLOBALS(dotenv)

DOTENV_VISIBILITY_HIDDEN ZEND_EXTERN_MODULE_GLOBALS(dotenv);

#endif /* PHP_DOTENV_H */
