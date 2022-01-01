#include "php_dotenv.h"
#include "utils.h"
#include <ext/standard/php_filestat.h>
#include <Zend/zend_smart_str.h>

static zend_bool is_readable(zend_string* dir) /* NOSONAR */
{
    zval rv;
#if PHP_VERSION_ID >= 80100
    php_stat(dir, FS_IS_R, &rv);
#else
    php_stat(ZSTR_VAL(dir), ZSTR_LEN(dir), FS_IS_R, &rv);
#endif
    return Z_TYPE(rv) == IS_TRUE;
}

zend_string* find_file_upward(const zend_string* dir, const zend_string* name)
{
    smart_str s = { NULL, 0 };
    zend_string* path = zend_string_init(ZSTR_VAL(dir), ZSTR_LEN(dir), 0);

    while (is_readable(path)) {
        smart_str_free(&s);
        smart_str_append(&s, path);
        if (ZSTR_LEN(path) != 1 || ZSTR_VAL(path)[0] != PHP_DIR_SEPARATOR) {
            smart_str_appendc(&s, PHP_DIR_SEPARATOR);
        }

        smart_str_append(&s, name);
        smart_str_0(&s);

        if (is_readable(s.s)) {
            zend_string_release(path);
            return s.s;
        }

        size_t path_len = ZSTR_LEN(path);
        size_t parent_len;
#ifdef PHP_WIN32
        parent_len = php_win32_ioutil_dirname(ZSTR_VAL(path), path_len);
#else
        parent_len = zend_dirname(ZSTR_VAL(path), path_len);
#endif
        ZSTR_LEN(path) = parent_len;
        if (parent_len == 0 || parent_len == path_len) {
            break;
        }
    }

    smart_str_free(&s);
    zend_string_release(path);
    return NULL;
}
