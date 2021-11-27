#ifndef BIN_TREE_CONFIG_H

#define BIN_TREE_CONFIG_H

#include "../debug_lib.h"

typedef char * Value_t;

// FILE *TREE_LOG_FILE_DEFAULT = stderr;

const size_t COLOR_MAX_LEN = 7 + 1;

const char root_color    [COLOR_MAX_LEN] = "#f4a1f7";
const char terminal_color[COLOR_MAX_LEN] = "#5ebfff";
const char default_color [COLOR_MAX_LEN] = "#63e69e";

const char left_color    [COLOR_MAX_LEN] = "#3cf71b";
const char right_color   [COLOR_MAX_LEN] = "#ff0000";

#endif