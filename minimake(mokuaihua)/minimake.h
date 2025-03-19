#ifndef MINIMAKE_H
#define MINIMAKE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>

// 公共宏定义
#define MAX_LINE_LEN 1024
#define MAX_TARGET_LEN 32
#define CLEANED_FILE "Minimake_cleared.mk"

// 公共函数声明
void print_help(const char *prog_name);

#endif