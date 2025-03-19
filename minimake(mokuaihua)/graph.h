#ifndef GRAPH_H
#define GRAPH_H

#include "check_syntax.h"

#define MAX_NODES 100

typedef struct {
    char name[MAX_TARGET_LEN];
    int in_degree;
    bool is_target;
    time_t last_modified;
} GraphNode;

typedef struct {
    GraphNode nodes[MAX_NODES];
    int adj_matrix[MAX_NODES][MAX_NODES];
    int node_count;
} DependencyGraph;

// 依赖图操作
DependencyGraph* build_dependency_graph();
void free_dependency_graph(DependencyGraph *graph);
bool topological_sort(DependencyGraph *graph, int *order);

#endif