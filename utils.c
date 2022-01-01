#include "php_dotenv.h"
#include "utils.h"
#include <main/SAPI.h>
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

static zend_string* get_cwd()
{
    char path[MAXPATHLEN];
    const char* ret = NULL;

#if HAVE_GETCWD
    ret = VCWD_GETCWD(path, MAXPATHLEN);
#elif HAVE_GETWD
    ret = VCWD_GETWD(path);
#endif

    if (ret) {
        return zend_string_init(ret, strlen(ret), 0);
    }

    return zend_string_init(ZEND_STRL("."), 0);
}

zend_string* find_file_upward(const zend_string* dir, const zend_string* name)
{
    smart_str s = { NULL, 0 };
    zend_string* path = zend_string_init(ZSTR_VAL(dir), ZSTR_LEN(dir), 0);

    while (is_readable(path)) {
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

        smart_str_free(&s);

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

    zend_string_release(path);
    return NULL;
}

zend_string* cli_find_env_file(const char* filename, zend_bool use_cwd)
{
    zend_string* fname = zend_string_init(filename, strlen(filename), 0);
    zend_string* dir;

    if (use_cwd) {
        dir = get_cwd();
    }
    else if (SG(request_info).path_translated) {
        dir = zend_string_init(SG(request_info).path_translated, strlen(SG(request_info).path_translated), 0);
#ifdef PHP_WIN32
        ZSTR_LEN(dir) = php_win32_ioutil_dirname(ZSTR_VAL(dir), ZSTR_LEN(dir));
#else
        ZSTR_LEN(dir) = zend_dirname(ZSTR_VAL(dir), ZSTR_LEN(dir));
#endif
    }
    else {
        zend_error(E_CORE_WARNING, "Unable to get the script directory");
        dir = zend_string_init(ZEND_STRL("."), 0);
    }

    zend_string* resolved = find_file_upward(dir, fname);
    zend_string_release(dir);
    zend_string_release(fname);

    return resolved;
}
