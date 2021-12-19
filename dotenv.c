#include "php_dotenv.h"
#include <assert.h>
#include <Zend/zend_ini_scanner.h>
#include <Zend/zend_string.h>
#include <ext/standard/info.h>

ZEND_DECLARE_MODULE_GLOBALS(dotenv);

PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("dotenv.file", "", PHP_INI_PERDIR, OnUpdateString, filename, zend_dotenv_globals, dotenv_globals)
PHP_INI_END()

static void ini_parser_callback(zval* key, zval* value, zval* index, int callback_type, void* arg)
{
    HashTable* tbl = (HashTable*)arg;

    if (callback_type == ZEND_INI_PARSER_ENTRY && key) {
        assert(Z_TYPE_P(key) == IS_STRING);
        assert(Z_TYPE_P(value) == IS_STRING);

        setenv(Z_STRVAL_P(key), Z_STRVAL_P(value), 1);
        /* Debug build of ZE does not like NULL in the third argument; we pass a valid pointer to keep ZE happy */
        zend_hash_update_ptr(tbl, Z_STR_P(key), Z_STR_P(key));
    }
}

static void load_env_file()
{
    struct zend_stat st;
    const char* filename = DOTENV_G(filename);

    if (filename && *filename && VCWD_STAT(filename, &st) == 0 && S_ISREG(st.st_mode)) {
        zend_file_handle fh;
#if PHP_VERSION_ID < 70400
        memset(&fh, 0, sizeof(fh));
        fh.handle.fp = VCWD_FOPEN(filename, "r");
        if (!fh.handle.fp) {
            zend_error(E_CORE_WARNING, "Cannot open file \"%s\" for reading", filename);
            return;
        }

        fh.type          = ZEND_HANDLE_FP;
        fh.opened_path   = NULL;
        fh.free_filename = 0;
        fh.filename      = filename;
#else
        zend_stream_init_filename(&fh, filename);
#endif
        zend_parse_ini_file(&fh, 1, ZEND_INI_SCANNER_RAW, ini_parser_callback, &DOTENV_G(entries));
#if PHP_VERSION_ID >= 80000
        zend_destroy_file_handle(&fh);
#endif
    }
}

static int clean_up_env(zval* zval_ptr, int num_args, va_list args, zend_hash_key* hash_key)
{
    if (hash_key->key) {
        unsetenv(hash_key->key->val);
    }

    return ZEND_HASH_APPLY_REMOVE;
}

static PHP_MINIT_FUNCTION(dotenv)
{
    REGISTER_INI_ENTRIES();
    return SUCCESS;
}

static PHP_MSHUTDOWN_FUNCTION(dotenv)
{
    UNREGISTER_INI_ENTRIES();
    return SUCCESS;
}

static PHP_RINIT_FUNCTION(dotenv)
{
    zend_hash_init(&DOTENV_G(entries), 32, NULL, NULL, 0);
    load_env_file();
    return SUCCESS;
}

static PHP_RSHUTDOWN_FUNCTION(dotenv)
{
    zend_hash_apply_with_arguments(&DOTENV_G(entries), clean_up_env, 0);
    zend_hash_destroy(&DOTENV_G(entries));
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
    PHP_RSHUTDOWN(dotenv),
    PHP_MINFO(dotenv),
    PHP_DOTENV_EXTVER,
    PHP_MODULE_GLOBALS(dotenv),
    NULL,
    NULL,
    NULL,
    STANDARD_MODULE_PROPERTIES_EX
};

ZEND_GET_MODULE(dotenv);
