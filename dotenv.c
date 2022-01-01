#include "php_dotenv.h"
#include <assert.h>
#include <Zend/zend_ini_scanner.h>
#include <Zend/zend_string.h>
#include <ext/standard/info.h>
#include "parse.h"

ZEND_DECLARE_MODULE_GLOBALS(dotenv);

PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("dotenv.file", "", PHP_INI_PERDIR, OnUpdateString, filename, zend_dotenv_globals, dotenv_globals)
    STD_PHP_INI_BOOLEAN("dotenv.overwrite", "0", PHP_INI_PERDIR, OnUpdateBool, overwrite_env, zend_dotenv_globals, dotenv_globals)
PHP_INI_END()

static void load_env_file(void)
{
    struct zend_stat st;
    const char* filename = DOTENV_G(filename);

    if (filename && *filename && VCWD_STAT(filename, &st) == 0 && S_ISREG(st.st_mode)) {
        HashTable ht;
        zend_hash_init(&ht, 32, NULL, ZVAL_PTR_DTOR, 0);

        parse_file(filename, &ht);

        zend_bool overwrite = DOTENV_G(overwrite_env);
        zend_string* key;
        zval* val;
        ZEND_HASH_FOREACH_STR_KEY_VAL(&ht, key, val)
            assert(Z_TYPE_P(val) == IS_STRING);
            if (overwrite || getenv(ZSTR_VAL(key)) == NULL) {
                setenv(ZSTR_VAL(key), Z_STRVAL_P(val), 1);
                zend_hash_update_ptr(&DOTENV_G(entries), key, key);
            }
        ZEND_HASH_FOREACH_END();
        zend_hash_destroy(&ht);
    }
}

static int clean_up_env(zval* zval_ptr, int num_args, va_list args, zend_hash_key* hash_key)
{
    if (hash_key->key) {
        unsetenv(hash_key->key->val);
    }

    return ZEND_HASH_APPLY_REMOVE;
}

static PHP_GINIT_FUNCTION(dotenv)
{
    dotenv_globals->re_ini_key_val = zend_string_init(ZEND_STRL("/^\\s*([\\w.-]+)\\s*=\\s*(.*)\\s*$/"), 1);
}

static PHP_GSHUTDOWN_FUNCTION(dotenv)
{
    zend_string_free(dotenv_globals->re_ini_key_val);
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

static PHP_FUNCTION(env_parse_file)
{
    char* filename;
    size_t filename_len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_PATH(filename, filename_len)
    ZEND_PARSE_PARAMETERS_END();

    array_init(return_value);
    parse_file(filename, Z_ARRVAL_P(return_value));
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_env_parse_file, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry env_functions[] = {
    ZEND_FE(env_parse_file, arginfo_env_parse_file)
    ZEND_FE_END
};

static zend_module_entry dotenv_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_DOTENV_EXTNAME,
    env_functions,
    PHP_MINIT(dotenv),
    PHP_MSHUTDOWN(dotenv),
    PHP_RINIT(dotenv),
    PHP_RSHUTDOWN(dotenv),
    PHP_MINFO(dotenv),
    PHP_DOTENV_EXTVER,
    PHP_MODULE_GLOBALS(dotenv),
    PHP_GINIT(dotenv),
    PHP_GSHUTDOWN(dotenv),
    NULL,
    STANDARD_MODULE_PROPERTIES_EX
};

ZEND_GET_MODULE(dotenv);
