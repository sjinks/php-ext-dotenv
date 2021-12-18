#include "php_dotenv.h"
#include <Zend/zend_ini_scanner.h>
#include <Zend/zend_string.h>
#include <ext/standard/info.h>

ZEND_DECLARE_MODULE_GLOBALS(dotenv);

PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("dotenv.file", "", PHP_INI_ALL, OnUpdateString, filename, zend_dotenv_globals, dotenv_globals)
PHP_INI_END()

static void ini_parser_callback(zval *key, zval *value, zval *index, int callback_type, void *arg)
{
    HashTable* ht = (HashTable*)arg;
    if (callback_type == ZEND_INI_PARSER_ENTRY && key) {
        Z_ADDREF_P(value);
        zend_hash_update(ht, Z_STR_P(key), value);
    }
}

static void load_env_file()
{
    struct zend_stat st;
    const char* filename = DOTENV_G(filename);

    if (filename && *filename && VCWD_STAT(filename, &st) == 0 && S_ISREG(st.st_mode)) {
        FILE* f = VCWD_FOPEN(filename, "r");
        if (!f) {
            zend_error(E_CORE_WARNING, "Cannot open file \"%s\" for reading", filename);
            return;
        }

        zend_file_handle fh;
        zend_stream_init_fp(&fh, f, filename);

        if (PG(auto_globals_jit)) {
#if PHP_VERSION_ID >= 80100
            zend_is_auto_global(ZSTR_KNOWN(ZEND_STR_AUTOGLOBAL_ENV));
#else
            zend_is_auto_global_str(ZEND_STRL("_ENV"));
#endif
        }

        zval* ptr = &PG(http_globals)[TRACK_VARS_ENV];
        if (ptr && Z_TYPE_P(ptr) == IS_ARRAY) {
            zend_parse_ini_file(&fh, 1, ZEND_INI_SCANNER_RAW, ini_parser_callback, Z_ARRVAL_P(ptr));
        }
        else {
            zend_error(E_CORE_WARNING, "_ENV is not yet initialized");
        }
    }
}

static PHP_MINIT_FUNCTION(dotenv)
{
    REGISTER_INI_ENTRIES();
}

static PHP_MSHUTDOWN_FUNCTION(dotenv)
{
    UNREGISTER_INI_ENTRIES();
}

static PHP_RINIT_FUNCTION(dotenv)
{
    load_env_file();
    return SUCCESS;
}

static PHP_MINFO_FUNCTION(dotenv)
{
    php_info_print_table_start();
    php_info_print_table_row(2, "dotenv extension", "enabled");
    php_info_print_table_row(2, "version", PHP_DOTENV_EXTVER);
    php_info_print_table_end();

    DISPLAY_INI_ENTRIES();
}

static zend_module_entry dotenv_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_DOTENV_EXTNAME,
    NULL,
    PHP_MINIT(dotenv),
    PHP_MSHUTDOWN(dotenv),
    PHP_RINIT(dotenv),
    NULL,
    PHP_MINFO(dotenv),
    PHP_DOTENV_EXTVER,
    PHP_MODULE_GLOBALS(dotenv),
    NULL,
    NULL,
    NULL,
    STANDARD_MODULE_PROPERTIES_EX
};

ZEND_GET_MODULE(dotenv);
