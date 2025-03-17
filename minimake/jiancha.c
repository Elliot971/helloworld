#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_LINE_LEN 512

// 语法检查函数
int check_makefile_syntax(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("Error opening Makefile");
        return -1;
    }

    char line[MAX_LINE_LEN];
    int line_num = 0;
    bool in_rule = false;  // 是否在规则定义中
    bool has_error = false;

    while (fgets(line, sizeof(line), fp)) {
        line_num++;
        size_t len = strlen(line);
        
        // 去除行尾换行符
        if (len > 0 && line[len-1] == '\n') line[--len] = '\0';

        // 空行跳过（不影响语法检查）
        if (len == 0) continue;

        // 检查目标行
        if (!in_rule && line[0] != '\t') {
            char* colon = strchr(line, ':');
            if (!colon) {
                printf("Line%d: Missing colon in target definition\n", line_num);
                has_error = true;
                continue;
            }
            
            // 验证目标格式（参考网页1核心规则）
            if (colon == line || colon == line + len -1) {
                printf("Line%d: Invalid target format\n", line_num);
                has_error = true;
            }
            in_rule = true;
        }
        // 检查命令格式
        else if (line[0] == '\t') {  // 必须严格使用Tab（网页5/8强调）
            if (!in_rule) {
                printf("Line%d: Command found before rule\n", line_num);
                has_error = true;
            }
        }
        // 非Tab非目标行（可能是错误格式）
        else {
            printf("Line%d: Invalid line format\n", line_num);
            has_error = true;
        }

        // 规则结束判断（参考网页9状态管理）
        if (in_rule && line[0] != '\t') {
            in_rule = false;
        }
    }

    fclose(fp);
    return has_error ? 1 : 0;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <makefile>\n", argv[0]);
        return 1;
    }

    int result = check_makefile_syntax(argv[1]);
    if (result == 0) {
        printf("Makefile syntax check passed!\n");
    }
    return result;
}