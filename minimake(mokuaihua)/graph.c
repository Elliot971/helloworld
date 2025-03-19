#include "graph.h"
#include "check_syntax.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

static int find_or_create_node(DependencyGraph *graph, const char *name, bool is_target) {
    for (int i = 0; i < graph->node_count; i++) {
        if (strcmp(graph->nodes[i].name, name) == 0) {
            return i;
        }
    }

    if (graph->node_count >= MAX_NODES) return -1;

    strncpy(graph->nodes[graph->node_count].name, name, MAX_TARGET_LEN);
    graph->nodes[graph->node_count].name[MAX_TARGET_LEN-1] = '\0';
    graph->nodes[graph->node_count].in_degree = 0;
    graph->nodes[graph->node_count].is_target = is_target;
    graph->nodes[graph->node_count].last_modified = get_file_mtime(name);
    return graph->node_count++;
}

DependencyGraph* build_dependency_graph() {
    DependencyGraph *graph = calloc(1, sizeof(DependencyGraph));
    
    // 构建所有节点
    for (int i = 0; i < rule_count; i++) {
        int target_idx = find_or_create_node(graph, rules[i].target, true);
        
        // 处理依赖关系
        for (int d = 0; d < rules[i].dep_count; d++) {
            const char *dep = rules[i].dependencies[d];
            bool is_dep_target = is_defined_target(dep);
            int dep_idx = find_or_create_node(graph, dep, is_dep_target);
            
            // 建立依赖关系
            if (graph->adj_matrix[dep_idx][target_idx] == 0) {
                graph->adj_matrix[dep_idx][target_idx] = 1;
                graph->nodes[target_idx].in_degree++;
            }
        }
    }
    return graph;
}

bool topological_sort(DependencyGraph *graph, int *order) {
    int queue[MAX_NODES];
    int front = 0, rear = 0;
    int sorted_count = 0;

    // 初始化队列
    for (int i = 0; i < graph->node_count; i++) {
        if (graph->nodes[i].in_degree == 0) {
            queue[rear++] = i;
        }
    }

    // Kahn算法
    while (front < rear) {
        int current = queue[front++];
        order[sorted_count++] = current;

        for (int i = 0; i < graph->node_count; i++) {
            if (graph->adj_matrix[current][i]) {
                if (--graph->nodes[i].in_degree == 0) {
                    queue[rear++] = i;
                }
            }
        }
    }

    return sorted_count == graph->node_count;
}

void free_dependency_graph(DependencyGraph *graph) {
    free(graph);
}