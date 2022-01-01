#ifndef D5A15088_DB24_46C5_8874_73F80B952A1E
#define D5A15088_DB24_46C5_8874_73F80B952A1E

#include "php_dotenv.h"

DOTENV_VISIBILITY_HIDDEN zend_string* find_file_upward(const zend_string* dir, const zend_string* name);
DOTENV_VISIBILITY_HIDDEN zend_string* cli_find_env_file(const char* filename, zend_bool use_cwd);

#endif /* D5A15088_DB24_46C5_8874_73F80B952A1E */
