#include "php_dotenv.h"
#include <assert.h>
#include <Zend/zend_ini_scanner.h>
#include <Zend/zend_string.h>
#include <main/php_main.h>
#include <ext/standard/info.h>
#include "parse.h"
#include "utils.h"

ZEND_DECLARE_MODULE_GLOBALS(dotenv);

PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("dotenv.file", "", PHP_INI_PERDIR, OnUpdateString, filename, zend_dotenv_globals, dotenv_globals)
    STD_PHP_INI_ENTRY("dotenv.cli.file", "", PHP_INI_PERDIR, OnUpdateString, cli_filename, zend_dotenv_globals, dotenv_globals)
    STD_PHP_INI_BOOLEAN("dotenv.cli.use_script_dir", "1", PHP_INI_PERDIR, OnUpdateBool, cli_use_script_dir, zend_dotenv_globals, dotenv_globals)
    STD_PHP_INI_BOOLEAN("dotenv.overwrite", "0", PHP_INI_PERDIR, OnUpdateBool, overwrite_env, zend_dotenv_globals, dotenv_globals)
PHP_INI_END()

static void load_env_file(void)
{
    struct zend_stat st;
    const char* filename = DOTENV_G(filename);
    zend_string* resolved = NULL;

    if (!strcmp(sapi_module.name, "cli")) {
        const char* cli_filename = DOTENV_G(cli_filename);
        if (cli_filename && *cli_filename) {
            resolved = cli_find_env_file(cli_filename, !DOTENV_G(cli_use_script_dir));
            filename = resolved ? ZSTR_VAL(resolved) : NULL;
        }
    }

    if (filename && *filename && VCWD_STAT(filename, &st) == 0 && S_ISREG(st.st_mode)) {
        HashTable ht;
        zend_hash_init(&ht, 32, NULL, ZVAL_PTR_DTOR, 0);

        parse_file(filename, NULL, &ht);

        zend_bool overwrite = DOTENV_G(overwrite_env);
        ZEND_HASH_FOREACH_STR_KEY_VAL(&ht, zend_string* key, const zval* val)
            assert(Z_TYPE_P(val) == IS_STRING);
            if (overwrite || getenv(ZSTR_VAL(key)) == NULL) {
                setenv(ZSTR_VAL(key), Z_STRVAL_P(val), 1);
                zend_hash_update_ptr(&DOTENV_G(entries), key, key);
            }
        ZEND_HASH_FOREACH_END();
        zend_hash_destroy(&ht);
    }

    if (resolved) {
        zend_string_release(resolved);
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
    dotenv_globals->re_ini_key_val = zend_string_init(ZEND_STRL("/^([\\w.-]+)\\s*=\\s*(.*+)$/"), 1);
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

static PHP_FUNCTION(dotenv_parse_file)
{
    char* filename;
    size_t filename_len;
    zval* zcontext = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_PATH(filename, filename_len)
        Z_PARAM_OPTIONAL
        Z_PARAM_RESOURCE_OR_NULL(zcontext)
    ZEND_PARSE_PARAMETERS_END();

    array_init(return_value);
    parse_file(filename, zcontext, Z_ARRVAL_P(return_value));
}

static PHP_FUNCTION(dotenv_setenv)
{
    HashTable* ht;
    zend_bool overwrite = DOTENV_G(overwrite_env);
    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ARRAY_HT(ht)
        Z_PARAM_OPTIONAL
        Z_PARAM_BOOL(overwrite)
    ZEND_PARSE_PARAMETERS_END();

    ZEND_HASH_FOREACH_STR_KEY_VAL(ht, zend_string* key, const zval* val)
        if (key && Z_TYPE_P(val) == IS_STRING && (overwrite || getenv(ZSTR_VAL(key)) == NULL)) {
            setenv(ZSTR_VAL(key), Z_STRVAL_P(val), 1);
            zend_hash_update_ptr(&DOTENV_G(entries), key, key);
        }
    ZEND_HASH_FOREACH_END();
}

static PHP_FUNCTION(dotenv_find_file_upward)
{
    zend_string* dir;
    zend_string* name;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_PATH_STR(dir)
        Z_PARAM_PATH_STR(name)
    ZEND_PARSE_PARAMETERS_END();

    zend_string* fname = find_file_upward(dir, name);
    if (fname) {
        RETURN_STR(fname);
    }

    RETURN_NULL();
}

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dotenv_parse_file, 0, 1, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
    ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, context, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dotenv_setenv, 0, 1, IS_VOID, 0)
    ZEND_ARG_ARRAY_INFO(0, env, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dotenv_find_file_upward, 0, 2, IS_STRING, 1)
    ZEND_ARG_TYPE_INFO(0, start_dir, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry dotenv_functions[] = {
    ZEND_FE(dotenv_parse_file, arginfo_dotenv_parse_file)
    ZEND_FE(dotenv_setenv, arginfo_dotenv_setenv)
    ZEND_FE(dotenv_find_file_upward, arginfo_dotenv_find_file_upward)
    ZEND_FE_END
};

static zend_module_entry dotenv_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_DOTENV_EXTNAME,
    dotenv_functions,
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
