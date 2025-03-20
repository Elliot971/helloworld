#include "build.h"
#include "check_syntax.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static const Rule* find_rule(const char *target) {
    for (int i = 0; i < rule_count; i++) {
        if (strcmp(rules[i].target, target) == 0) {
            return &rules[i];
        }
    }
    return NULL;
}

static bool needs_rebuild(const Rule *rule) {
    time_t target_time = get_file_mtime(rule->target);
    
    // 目标不存在时需要构建
    if (target_time == 0) return true;

    // 检查所有依赖项
    for (int i = 0; i < rule->dep_count; i++) {
        const char *dep = rule->dependencies[i];
        time_t dep_time = get_file_mtime(dep);
        
        if (dep_time > target_time) {
            return true;
        }
    }
    return false;
}

static int execute_commands(const Rule *rule) {
    for (int i = 0; i < rule->cmd_count; i++) {
        printf("Executing: %s\n", rule->commands[i]);
        if (system(rule->commands[i]) != 0) {
            fprintf(stderr, "Command failed: %s\n", rule->commands[i]);
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

int build_target(const char *target) {
    if (!find_rule(target) && !file_exists(target)) {
        fprintf(stderr, "Error: Target '%s' not found\n", target);
        return EXIT_FAILURE;
    }

    DependencyGraph *graph = build_dependency_graph();
    if (!graph) return EXIT_FAILURE;

    int order[MAX_NODES];
    if (!topological_sort(graph, order)) {
        fprintf(stderr, "Error: Cyclic dependencies detected\n");
        free_dependency_graph(graph);
        return EXIT_FAILURE;
    }

    int result = EXIT_SUCCESS;
    for (int i = 0; i < graph->node_count; i++) {
        int idx = order[i];
        const char *name = graph->nodes[idx].name;

        // 只处理定义的目标
        if (!graph->nodes[idx].is_target) continue;

        const Rule *rule = find_rule(name);
        if (!rule) {
            fprintf(stderr, "Internal error: Missing rule for '%s'\n", name);
            result = EXIT_FAILURE;
            break;
        }

        // 检查依赖是否就绪
        for (int d = 0; d < rule->dep_count; d++) {
            const char *dep = rule->dependencies[d];
            if (!file_exists(dep) && !is_defined_target(dep)) {
                fprintf(stderr, "Missing dependency: %s\n", dep);
                result = EXIT_FAILURE;
                break;
            }
        }
        if (result != EXIT_SUCCESS) break;

        // 执行构建
        if (needs_rebuild(rule)) {
            printf("Building: %s\n", name);
            if (execute_commands(rule) != EXIT_SUCCESS) {
                result = EXIT_FAILURE;
                break;
            }
        } else {
            printf("Skipping up-to-date target: %s\n", name);
        }
    }

    free_dependency_graph(graph);
    return result;
}