#ifndef PHP_DOTENV_H
#define PHP_DOTENV_H

#include <main/php.h>

#define PHP_DOTENV_EXTNAME     "dotenv"
#define PHP_DOTENV_EXTVER      "0.5.0"

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
	char* cli_filename;
	zend_string* re_ini_key_val;
	zend_bool overwrite_env;
	zend_bool cli_use_script_dir;
ZEND_END_MODULE_GLOBALS(dotenv)

DOTENV_VISIBILITY_HIDDEN ZEND_EXTERN_MODULE_GLOBALS(dotenv);

#endif /* PHP_DOTENV_H */
