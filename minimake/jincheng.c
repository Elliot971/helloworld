/*阶段四的任务我在原来阶段三的代码基础上进行了修改，想替换原来的system()调用，按照要求可以在execute_single_command函数中，使用fork创建子进程。
在子进程中，解析命令字符串为参数数组。
调用execvp执行命令，处理可能的错误，比如命令不存在。
在父进程中，使用waitpid等待子进程结束，检查退出状态。
如果任何命令失败，立即返回错误，中断构建。如果验证阶段四的任务，需要将源文件名换成build.c*/
#include "build.h"
#include "check_syntax.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

// 新增：执行单个命令的完整实现
static int execute_single_command(const char *cmd) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("[ERROR] fork");
        return -1;
    }
    
    if (pid == 0) { // 子进程
        // 使用sh -c执行命令以支持完整shell语法
        char *argv[] = {"sh", "-c", (char*)cmd, NULL};
        execvp(argv[0], argv);
        
        // 只有exec失败才会执行到这里
        fprintf(stderr, "[ERROR] execvp: %s (%s)\n", strerror(errno), cmd);
        _exit(EXIT_FAILURE); // 使用_exit避免刷新stdio缓冲区
    }

    // 父进程
    int status;
    while (waitpid(pid, &status, 0) == -1) {
        if (errno != EINTR) { // 处理信号中断
            perror("[ERROR] waitpid");
            return -1;
        }
    }

    // 解析退出状态
    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        if (exit_code != 0) {
            fprintf(stderr, "[FAIL] Command exited %d: %s\n", exit_code, cmd);
            return -1;
        }
    } else if (WIFSIGNALED(status)) {
        fprintf(stderr, "[FAIL] Command killed by signal %d: %s\n", 
               WTERMSIG(status), cmd);
        return -1;
    }
    return 0;
}

// 重构后的命令执行逻辑
static int execute_commands(const Rule *rule) {
    for (int i = 0; i < rule->cmd_count; i++) {
        printf("[RUN] %s\n", rule->commands[i]);
        if (execute_single_command(rule->commands[i]) != 0) {
            return -1; // 关键错误中断机制
        }
    }
    return 0;
}

// 修改后的构建流程（保留原有依赖检查）
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

        if (needs_rebuild(rule)) {
            printf("[BUILD] %s\n", rule->target);
            if (execute_commands(rule) != 0) {
                result = EXIT_FAILURE;
                break; // 关键修改点：立即中断构建
            }
        }
    }
    
    free_dependency_graph(graph);
     return result;
}