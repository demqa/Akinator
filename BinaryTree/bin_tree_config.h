#ifndef BIN_TREE_CONFIG_H

#define BIN_TREE_CONFIG_H

typedef char * Value_t;

// FILE *TREE_LOG_FILE_DEFAULT = stderr;

const size_t COLOR_MAX_LEN = 7 + 1;

const char root_color    [COLOR_MAX_LEN] = "#f4a1f7";
const char internal_color[COLOR_MAX_LEN] = "#ba092d";
const char default_color [COLOR_MAX_LEN] = "#6bc79c";

const char left_color    [COLOR_MAX_LEN] = "#3bfc2d";
const char right_color   [COLOR_MAX_LEN] = "#ff0000";

#endif