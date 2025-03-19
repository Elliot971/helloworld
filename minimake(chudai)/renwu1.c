#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_help(const char *prog_name) {
    printf("Usage: %s [--help] [target]\n", prog_name);
    printf("Options:\n");
    printf("  --help      Display this help message and exit.\n");
}

int main(int argc, char *argv[]) {
    // 检查是否存在 --help 参数
    int help_flag = 0;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--help") == 0) {
            help_flag = 1;
            break;
        }
    }
    if (help_flag) {
        print_help(argv[0]);
        exit(EXIT_SUCCESS);
    }

    // 检查无效参数（以 - 开头）
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            fprintf(stderr, "Error: Invalid argument '%s'\n", argv[i]);
            fprintf(stderr, "Usage: %s [--help] [target]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // 检查是否提供了目标参数
    if (argc < 2) {
        fprintf(stderr, "Error: Missing target\n");
        fprintf(stderr, "Usage: %s [--help] [target]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 参数解析成功，后续处理（此处省略）
    printf("Building target: %s\n", argv[1]);
    return EXIT_SUCCESS;
}