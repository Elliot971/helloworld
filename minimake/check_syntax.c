#include "check_syntax.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

// 全局规则存储
Rule rules[MAX_RULES];
int rule_count = 0;

void add_dependency(Rule *rule, const char *dep) {
    if (rule->dep_count < MAX_DEPS) {
        strncpy(rule->dependencies[rule->dep_count], dep, MAX_TARGET_LEN);
        rule->dependencies[rule->dep_count][MAX_TARGET_LEN-1] = '\0';
        rule->dep_count++;
    }
}

void add_command(Rule *rule, const char *cmd) {
    if (rule->cmd_count < MAX_CMDS) {
        strncpy(rule->commands[rule->cmd_count], cmd, MAX_LINE_LEN);
        rule->commands[rule->cmd_count][MAX_LINE_LEN-1] = '\0';
        rule->cmd_count++;
    }
}

int check_makefile_syntax(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) return EXIT_FAILURE;

    char line[MAX_LINE_LEN];
    int line_num = 0;
    Rule *current_rule = NULL;
    bool has_error = false;

    while (fgets(line, sizeof(line), fp) && !has_error) {
        line_num++;
        line[strcspn(line, "\n")] = '\0';
        trim_trailing_spaces(line);
        if (is_blank_line(line)) continue;

        // 目标行解析
        if (current_rule == NULL && strchr(line, ':') != NULL) {
            char *colon = strchr(line, ':');
            if (colon == line || *(colon+1) == '\0') {
                printf("Line %d: Invalid target format\n", line_num);
                has_error = true;
                continue;
            }

            *colon = '\0';
            char *target = line;
            char *deps = colon + 1;
            trim_trailing_spaces(target);
            trim_trailing_spaces(deps);

            // 检查重复目标
            for (int i = 0; i < rule_count; i++) {
                if (strcmp(rules[i].target, target) == 0) {
                    printf("Line %d: Duplicate target '%s'\n", line_num, target);
                    has_error = true;
                    break;
                }
            }

            if (rule_count >= MAX_RULES) {
                printf("Line %d: Exceed max rules limit\n", line_num);
                has_error = true;
                continue;
            }

            // 初始化新规则
            current_rule = &rules[rule_count];
            strncpy(current_rule->target, target, MAX_TARGET_LEN);
            current_rule->target[MAX_TARGET_LEN-1] = '\0';
            current_rule->dep_count = 0;
            current_rule->cmd_count = 0;
            current_rule->line_number = line_num;

            // 解析依赖
            char *dep = strtok(deps, " ");
            while (dep != NULL) {
                add_dependency(current_rule, dep);
                dep = strtok(NULL, " ");
            }

            rule_count++;
        }
        // 命令解析
        else if (line[0] == '\t') {
            if (current_rule == NULL) {
                printf("Line %d: Command before target\n", line_num);
                has_error = true;
                continue;
            }
            add_command(current_rule, line + 1);
        }
        else {
            current_rule = NULL;
        }
    }

    fclose(fp);
    return has_error ? EXIT_FAILURE : EXIT_SUCCESS;

 // 新增静态依赖检查
int i, d;
for (i=0; i<rule_count; i++) {
    for (d=0; d<rules[i].dep_count; d++) {
        const char* dep = rules[i].dependencies[d];
        if (!file_exists(dep) && !is_defined_target(dep)) {
            printf("Line %d: Invalid dependency '%s'\n", 
                   rules[i].line_number, dep);
            has_error = true;
        }
    }
}
}

bool is_defined_target(const char *name) {
    for (int i = 0; i < rule_count; i++) {
        if (strcmp(rules[i].target, name) == 0) return true;
    }
    return false;
}