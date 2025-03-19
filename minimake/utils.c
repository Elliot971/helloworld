#include "utils.h"

void trim_trailing_spaces(char *line) {
    char *end = line + strlen(line) - 1;
    while (end > line && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
}

bool is_blank_line(const char *line) {
    while (*line) if (!isspace(*line++)) return false;
    return true;
}

bool file_exists(const char *filename) {
    struct stat st;
    return stat(filename, &st) == 0;
}

time_t get_file_mtime(const char *filename) {
    struct stat st;
    return stat(filename, &st) == 0 ? st.st_mtime : 0;
}