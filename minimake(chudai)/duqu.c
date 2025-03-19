#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define CLEANED_FILE "Minimake_cleared.mk"

// 判断是否空行（仅含空白字符）
bool is_blank_line(const char *line) {
    while (*line) {
        if (!isspace((unsigned char)*line))
            return false;
        line++;
    }
    return true;
}

// 去除行尾空格/制表符
void trim_trailing_spaces(char *line) {
    int len = strlen(line);
    while (len > 0 && (line[len-1] == ' ' || line[len-1] == '\t' || line[len-1] == '\r'))
        line[--len] = '\0';
}

// 预处理主函数
int preprocess_makefile(bool verbose) {
    FILE *fp = fopen("./Makefile", "r");
    if (!fp) {
        perror("[ERROR] Makefile not found");
        return EXIT_FAILURE;
    }

    FILE *out = verbose ? fopen(CLEANED_FILE, "w") : NULL;
    char line[1024];
    int processed_lines = 0;

    while (fgets(line, sizeof(line), fp)) {
        // 去除注释（含行内注释）
        char *comment = strchr(line, '#');
        if (comment) *comment = '\0';

        // 去除行尾空格
        trim_trailing_spaces(line);

        // 过滤空行
        if (is_blank_line(line)) continue;

        // 调试模式输出
        if (verbose) {
            fprintf(out, "%s\n", line);
            printf("[DEBUG] Processed: %s\n", line);
        }
        processed_lines++;
    }

    fclose(fp);
    if (verbose) fclose(out);
    return processed_lines > 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

int main(int argc,char **argv) {
    bool verbose = (argc > 1 && (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--verbose") == 0));
    
    int result = preprocess_makefile(verbose);
    if (result == EXIT_SUCCESS) {
        printf("Makefile preprocessing completed%s\n", 
              verbose ? ", cleaned version saved to Minimake_cleared.mk" : "");
    }
    return result;
}
