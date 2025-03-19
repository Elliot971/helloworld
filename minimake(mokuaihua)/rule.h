#ifndef RULE_H
#define RULE_H

#include "utils.h"

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

extern int rule_count;
extern Rule rules[MAX_RULES];

// 规则操作接口
void init_rules();
void add_rule(const char *target, int line_num);
void add_dependency(Rule *rule, const char *dep);
void add_command(Rule *rule, const char *cmd);
bool is_defined_target(const char *name);
const Rule* get_rule(const char *target);

#endif