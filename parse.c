#include "php_dotenv.h"
#include "parse.h"
#include <assert.h>
#include <main/php_streams.h>
#include <ext/pcre/php_pcre.h>
#include <ext/standard/file.h>
#include <ext/standard/php_string.h>

#if PHP_VERSION_ID < 70300
static void php_pcre_pce_incref(pcre_cache_entry* pce)
{
    ++pce->refcount;
}

static void php_pcre_pce_decref(pcre_cache_entry* pce)
{
    --pce->refcount;
}
#endif

#if PHP_VERSION_ID >= 70400
#   define MAYBE_CONST const
#else
#   define MAYBE_CONST
#endif

static int parse_line(MAYBE_CONST char* line, size_t line_len, zend_string** key, zend_string** val)
{
    assert(key != NULL);
    assert(val != NULL);

    *key = NULL;
    *val = NULL;

    zend_string* regex    = DOTENV_G(re_ini_key_val);
    pcre_cache_entry* pce = pcre_get_compiled_regex_cache(regex);
    if (!pce) {
        return FAILURE;
    }

    zval rv;
    zval subpats;

    ZVAL_NULL(&rv);
    ZVAL_NULL(&subpats);

#if PHP_VERSION_ID < 70400
    php_pcre_pce_incref(pce);
    php_pcre_match_impl(pce, line, line_len, &rv, &subpats, 0, 0, 0, 0);
    php_pcre_pce_decref(pce);
#else
    zend_string* s = zend_string_init(line, line_len, 0);
    php_pcre_pce_incref(pce);
#   if PHP_VERSION_ID < 80400
    php_pcre_match_impl(pce, s, &rv, &subpats, 0, 0, 0, 0);
#   else
    php_pcre_match_impl(pce, s, &rv, &subpats, 0, 0, 0);
#   endif
    php_pcre_pce_decref(pce);
    zend_string_free(s);
#endif

    if (Z_TYPE(rv) == IS_LONG && Z_LVAL(rv) == 1 && Z_TYPE(subpats) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL(subpats)) == 3) {
        zval* k = zend_hash_index_find(Z_ARRVAL(subpats), 1);
        zval* v = zend_hash_index_find(Z_ARRVAL(subpats), 2);

        assert(Z_TYPE_P(k) == IS_STRING);
        assert(Z_TYPE_P(v) == IS_STRING);

        *key = Z_STR_P(k);
        zend_string_addref(*key);

        zend_bool is_single_quoted = 0;
        zend_bool is_double_quoted = 0;

        size_t len          = Z_STRLEN_P(v);
        MAYBE_CONST char* p = Z_STRVAL_P(v);

        if (len >= 2) {
            is_single_quoted = p[0] == '\'' && p[len-1] == '\'';
            is_double_quoted = p[0] == '"' && p[len-1] == '"';
        }

        if (is_double_quoted) {
            *val = php_str_to_str(p + 1, len - 2, ZEND_STRL("\\n"), ZEND_STRL("\n"));
        }
        else if (is_single_quoted) {
            *val = zend_string_init(p + 1, len - 2, 0);
        }
        else {
            *val = php_trim(Z_STR_P(v), NULL, 0, 3);
        }
    }

    zval_ptr_dtor(&rv);
    zval_ptr_dtor(&subpats);
    return SUCCESS;
}

void parse_file(const char* fname, zval* zcontext, HashTable* res)
{
    php_stream_context* context = php_stream_context_from_zval(zcontext, 0);
    php_stream* stream = php_stream_open_wrapper_ex(fname, "r", REPORT_ERRORS, NULL, context);
    char* buf;
    size_t line_len;
    int rc = SUCCESS;

    while (rc == SUCCESS && (buf = php_stream_get_line(stream, NULL, 0, &line_len)) != NULL) { /* NOSONAR */
        zend_string* key;
        zend_string* val;
        MAYBE_CONST char* p = buf;
        MAYBE_CONST char* e = buf + line_len;
        while (p != e && isspace(*p)) {
            ++p;
        }

        while (p != e && isspace(*(e - 1))) {
            --e;
        }

        rc = parse_line(p, e - p, &key, &val);
        efree(buf);

        if (rc == SUCCESS && key) {
            zval v;
            ZVAL_STR(&v, val);
            zend_hash_add(res, key, &v);
            zend_string_release(key);
        }
    }

    php_stream_free(stream, PHP_STREAM_FREE_KEEP_RSRC | PHP_STREAM_FREE_CLOSE);
}
