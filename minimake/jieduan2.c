// filepath: /home/elliot/projects/helloworld/minimake/jieduan2.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>  // 添加这个头文件

#define MAX_LINE_LEN 1024
#define CLEANED_FILE "Minimake_cleared.mk"

/*======= 新增数据结构 =======*/
#define MAX_RULES 50
#define MAX_DEPS 20
#define MAX_CMDS 10
#define MAX_TARGET_LEN 32

typedef struct {
    char target[MAX_TARGET_LEN];
    char dependencies[MAX_DEPS][MAX_TARGET_LEN];
    int dep_count;
    char commands[MAX_CMDS][MAX_LINE_LEN];
    int cmd_count;
    int line_number;
} Rule;

static Rule rules[MAX_RULES];
static int rule_count = 0;

// 函数声明
static bool is_blank_line(const char *line);
static void trim_trailing_spaces(char *line);
static bool file_exists(const char *filename);
static bool is_defined_target(const char *name);

/*======= 整合到语法检查模块 =======*/
int check_makefile_syntax(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) return EXIT_FAILURE;

    char line[MAX_LINE_LEN];
    int line_num = 0;
    bool in_rule = false;
    bool has_error = false;
    Rule *current_rule = NULL;

    while (fgets(line, sizeof(line), fp)) {
        line_num++;
        line[strcspn(line, "\n")] = '\0';
        trim_trailing_spaces(line);
        if (is_blank_line(line)) continue;

        /*======= 阶段二新增解析逻辑 =======*/
        // 目标行检测
        if (!in_rule && strchr(line, ':') != NULL) {
            // 解析目标
            char *colon = strchr(line, ':');
            *colon = '\0';
            char *target = line;
            char *deps = colon + 1;
            trim_trailing_spaces(target);
            trim_trailing_spaces(deps);

            // 重复目标检查（阶段二核心检查1）
            for (int i = 0; i < rule_count; i++) {
                if (strcmp(rules[i].target, target) == 0) {
                    printf("Line %d: Duplicate target definition '%s'\n", line_num, target);
                    has_error = true;
                }
            }

            // 初始化新规则
            if (rule_count < MAX_RULES) {
                current_rule = &rules[rule_count];
                strncpy(current_rule->target, target, MAX_TARGET_LEN);
                current_rule->dep_count = 0;
                current_rule->cmd_count = 0;
                current_rule->line_number = line_num;

                // 解析依赖项
                char *dep = strtok(deps, " ");
                while (dep && current_rule->dep_count < MAX_DEPS) {
                    strncpy(current_rule->dependencies[current_rule->dep_count++], dep, MAX_TARGET_LEN);
                    dep = strtok(NULL, " ");
                }
                rule_count++;
            }
            in_rule = true;
        }
        // 命令行检测
        else if (in_rule && line[0] == '\t') {
            if (current_rule && current_rule->cmd_count < MAX_CMDS) {
                strncpy(current_rule->commands[current_rule->cmd_count++], line + 1, MAX_LINE_LEN);
            }
        }
        else {
            in_rule = false;
            current_rule = NULL;
        }

        /*======= 保留阶段一基础检查 =======*/
        if (!in_rule && line[0] != '\t' && strchr(line, ':') == NULL) {
            printf("Line %d: Invalid line format\n", line_num);
            has_error = true;
        }
    }

    /*======= 阶段二新增依赖检查 =======*/
    if (!has_error) {
        for (int i = 0; i < rule_count; i++) {
            for (int j = 0; j < rules[i].dep_count; j++) {
                const char *dep = rules[i].dependencies[j];
                if (!file_exists(dep) && !is_defined_target(dep)) {
                    printf("Line %d: Invalid dependency '%s'\n", 
                          rules[i].line_number, dep);
                    has_error = true;
                }
            }
        }
    }

    fclose(fp);
    return has_error ? EXIT_FAILURE : EXIT_SUCCESS;
}

/*======= 新增辅助函数 =======*/
static bool is_blank_line(const char *line) {
    while (*line) {
        if (!isspace((unsigned char)*line)) {
            return false;
        }
        line++;
    }
    return true;
}

static void trim_trailing_spaces(char *line) {
    char *end = line + strlen(line) - 1;
    while (end > line && isspace((unsigned char)*end)) {
        end--;
    }
    end[1] = '\0';
}

static bool file_exists(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp) { fclose(fp); return true; }
    return false;
}

static bool is_defined_target(const char *name) {
    for (int i = 0; i < rule_count; i++) {
        if (strcmp(rules[i].target, name) == 0) return true;
    }
    return false;
}