#ifndef SYNTAX_H
#define SYNTAX_H

#include "minimake.h"

// 规则数据结构
#define MAX_RULES 50
#define MAX_DEPS 20
#define MAX_CMDS 10

typedef struct {
    char target[MAX_TARGET_LEN];
    char dependencies[MAX_DEPS][MAX_TARGET_LEN];
    int dep_count;
    char commands[MAX_CMDS][MAX_LINE_LEN];
    int cmd_count;
    int line_number;
} Rule;

// 全局变量声明
extern Rule rules[];
extern int rule_count;

// 函数声明
int check_makefile_syntax(const char* filename);
bool is_defined_target(const char *name);

#endif