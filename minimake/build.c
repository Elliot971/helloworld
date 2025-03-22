/*阶段四的任务我在原来阶段三的代码基础上进行了修改，想替换原来的system()调用，按照要求可以在execute_single_command函数中，使用fork创建子进程。
在子进程中，解析命令字符串为参数数组。
调用execvp执行命令，处理可能的错误，比如命令不存在。
在父进程中，使用waitpid等待子进程结束，检查退出状态。
如果任何命令失败，立即返回错误，中断构建。如果验证阶段四的任务，需要将源文件名换成build.c*/
#include "build.h"
#include "check_syntax.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

// 静态函数声明
static const Rule* find_rule(const char *target);
static bool needs_rebuild(const Rule *rule);

/* 新增函数：安全执行单个命令 */
static int execute_single_command(const char *cmd, const char *target) {
    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "minimake: ​*** fork error: %s\n", strerror(errno));
        return -1;
    }
    
    if (pid == 0) { // 子进程
        // 使用sh执行以支持shell特性（如通配符、管道等）
        char *argv[] = {"sh", "-c", (char*)cmd, NULL};
        execvp(argv[0], argv);
        
        // 若执行到此说明exec失败
        fprintf(stderr, "minimake: ​*** exec error: %s\n", strerror(errno));
        _exit(EXIT_FAILURE); // 必须用_exit避免重复刷新缓冲区
    }

    // 父进程等待并处理状态
    int status;
    while (waitpid(pid, &status, 0) == -1) {
        if (errno != EINTR) { // 处理信号中断
            fprintf(stderr, "minimake: ​*** waitpid error: %s\n", strerror(errno));
            return -1;
        }
    }

    // 解析子进程状态
    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        if (exit_code != 0) {
            fprintf(stderr, "minimake: ​*** [%s] Error %d (command: '%s')\n",
                    target, exit_code, cmd);
            return -1;
        }
    } else if (WIFSIGNALED(status)) {
        fprintf(stderr, "minimake: ​*** [%s] Killed by signal %d (command: '%s')\n",
                target, WTERMSIG(status), cmd);
        return -1;
    }
    return 0;
}

/* 重构命令执行逻辑 */
static int execute_commands(const Rule *rule) {
    for (int i = 0; i < rule->cmd_count; i++) {
        printf("minimake: Executing '%s'\n", rule->commands[i]);
        
        // 执行失败立即返回（关键错误中断点）
        if (execute_single_command(rule->commands[i], rule->target) != 0) {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

/* 修改后的构建入口函数 */
int build_target(const char *target) {
    // 目标存在性检查（保持原有逻辑）
    if (!find_rule(target) && !file_exists(target)) {
        fprintf(stderr, "minimake: ​*** No rule to make target '%s'\n", target);
        return EXIT_FAILURE;
    }

    // 构建依赖图（保持原有逻辑）
    DependencyGraph *graph = build_dependency_graph();
    if (!graph) return EXIT_FAILURE;

    // 拓扑排序（保持原有逻辑）
    int order[MAX_NODES];
    if (!topological_sort(graph, order)) {
        fprintf(stderr, "minimake: ​*** Cyclic dependency detected\n");
        free_dependency_graph(graph);
        return EXIT_FAILURE;
    }

    int result = EXIT_SUCCESS;
    for (int i = 0; i < graph->node_count; i++) {
        int idx = order[i];
        const char *name = graph->nodes[idx].name;

        // 跳过非目标节点（如原始文件）
        if (!graph->nodes[idx].is_target) continue;

        // 查找对应规则
        const Rule *rule = find_rule(name);
        if (!rule) {
            fprintf(stderr, "minimake: ​*** Internal error: no rule for '%s'\n", name);
            result = EXIT_FAILURE;
            break;
        }

        // 检查依赖可用性（新增详细错误信息）
        for (int d = 0; d < rule->dep_count; d++) {
            const char *dep = rule->dependencies[d];
            if (!file_exists(dep) && !is_defined_target(dep)) {
                fprintf(stderr, "minimake: ​*** Missing dependency '%s' needed by target '%s'\n", 
                       dep, name);
                result = EXIT_FAILURE;
                break;
            }
        }
        if (result != EXIT_SUCCESS) break;

        // 判断是否需要重建
        if (needs_rebuild(rule)) {
            printf("minimake: Building target '%s'\n", name);
            if (execute_commands(rule) != EXIT_SUCCESS) {
                result = EXIT_FAILURE;
                break; // 关键修改：命令失败立即终止
            }
        } else {
            printf("minimake: '%s' is up to date\n", name);
        }
    }
    
    free_dependency_graph(graph);
    return result;
}


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
    if (target_time == 0) return true;

    for (int i = 0; i < rule->dep_count; i++) {
        const char *dep = rule->dependencies[i];
        time_t dep_time = get_file_mtime(dep);
        if (dep_time > target_time) return true;
    }
    return false;
}