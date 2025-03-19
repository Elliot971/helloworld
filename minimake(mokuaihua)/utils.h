#ifndef UTILS_H
#define UTILS_H

#include "minimake.h"

// 字符串处理
void trim_trailing_spaces(char *line);
bool is_blank_line(const char *line);

// 文件操作
bool file_exists(const char *filename);
time_t get_file_mtime(const char *filename);

#endif