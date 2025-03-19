#include "rule.h"
#include <string.h>

static Rule rules[MAX_RULES];
static int rule_count = 0;

void init_rules() {
    rule_count = 0;
    memset(rules, 0, sizeof(rules));
}

void add_rule(const char *target, int line_num) {
    if (rule_count < MAX_RULES) {
        Rule *r = &rules[rule_count++];
        strncpy(r->target, target, MAX_TARGET_LEN);
        r->line_number = line_num;
        r->dep_count = r->cmd_count = 0;
    }
}

void add_dependency(Rule *rule, const char *dep) {
    if (rule->dep_count < MAX_DEPS) {
        strncpy(rule->dependencies[rule->dep_count++], dep, MAX_TARGET_LEN);
    }
}

void add_command(Rule *rule, const char *cmd) {
    if (rule->cmd_count < MAX_CMDS) {
        strncpy(rule->commands[rule->cmd_count++], cmd, MAX_LINE_LEN);
    }
}

bool is_defined_target(const char *name) {
    for (int i = 0; i < rule_count; i++) {
        if (strcmp(rules[i].target, name) == 0) return true;
    }
    return false;
}

const Rule* get_rule(const char *target) {
    for (int i = 0; i < rule_count; i++) {
        if (strcmp(rules[i].target, target) == 0) return &rules[i];
    }
    return NULL;
}