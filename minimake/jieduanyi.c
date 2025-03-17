#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_LINE_LEN 1024
#define CLEANED_FILE "Minimake_cleared.mk"

// 公共函数声明
bool is_blank_line(const char *line);
void trim_trailing_spaces(char *line);
int preprocess_makefile(bool verbose);
int check_makefile_syntax(const char* filename);
void print_help(const char *prog_name);

// 构建执行模块
int build_target(const char *target) {
    printf("Building target: %s\n", target);
    return EXIT_SUCCESS;
}

// 主函数
int main(int argc,char **argv) {
    if (argc < 2 || strcmp(argv[1],"--help") == 0 || strcmp(argv[1],"-h") == 0) {
        print_help(argv[0]);
        return EXIT_SUCCESS;
    }

    const char *subcmd = argv[1];
    if (strcmp(subcmd, "preprocess") == 0) {
        bool verbose = (argc > 2 && (strcmp(argv[2], "-v") == 0 || strcmp(argv[2], "--verbose") == 0));
        return preprocess_makefile(verbose);
    } 
    else if (strcmp(subcmd, "check") == 0) {
        return check_makefile_syntax("Makefile");
    }
    else if (strcmp(subcmd, "build") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Error: Missing build target\n");
            return EXIT_FAILURE;
        }
        if (preprocess_makefile(false) != EXIT_SUCCESS ||
            check_makefile_syntax(CLEANED_FILE) != EXIT_SUCCESS) {
            return EXIT_FAILURE;
        }
        return build_target(argv[2]);
    }
    else {
        fprintf(stderr, "Error: Invalid subcommand '%s'\n", subcmd);
        print_help(argv[0]);
        return EXIT_FAILURE;
    }
}

// 公共函数实现
bool is_blank_line(const char *line) {
    while (*line) {
        if (!isspace((unsigned char)*line))
            return false;
        line++;
    }
    return true;
}

void trim_trailing_spaces(char *line) {
    int len = strlen(line);
    while (len > 0 && (line[len-1] == ' ' || line[len-1] == '\t' || line[len-1] == '\r'))
        line[--len] = '\0';
}

int preprocess_makefile(bool verbose) {
    FILE *fp = fopen("Makefile", "r");
    if (!fp) {
        perror("[ERROR] Makefile not found");
        return EXIT_FAILURE;
    }

    FILE *out = verbose ? fopen(CLEANED_FILE, "w") : NULL;
    char line[MAX_LINE_LEN];
    int processed = 0;

    while (fgets(line, sizeof(line), fp)) {
        // 处理换行符
        line[strcspn(line, "\n")] = '\0';
        
        char *comment = strchr(line, '#');
        if (comment) *comment = '\0';
        
        trim_trailing_spaces(line);
        
        if (is_blank_line(line)) continue;

        if (verbose) {
            if (out) fprintf(out, "%s\n", line);
            printf("[DEBUG] %s\n", line);
        }
        processed++;
    }

    fclose(fp);
    if (verbose && out) fclose(out);
    return processed > 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

int check_makefile_syntax(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    char line[MAX_LINE_LEN];
    int line_num = 0;
    bool in_rule = false;
    bool has_error = false;

    while (fgets(line, sizeof(line), fp)) {
        line_num++;
        line[strcspn(line, "\n")] = '\0';
        size_t len = strlen(line);

        if (len == 0) continue;

        if (!in_rule && line[0] != '\t') {
            char* colon = strchr(line, ':');
            if (!colon) {
                printf("Line %d: Missing colon in target definition\n", line_num);
                has_error = true;
                continue;
            }
            
            if (colon == line || colon == line + len -1) {
                printf("Line %d: Invalid target format\n", line_num);
                has_error = true;
            }
            in_rule = true;
        }
        else if (line[0] == '\t') {
            if (!in_rule) {
                printf("Line %d: Command found before rule\n", line_num);
                has_error = true;
            }
        }
        else {
            printf("Line %d: Invalid line format\n", line_num);
            has_error = true;
        }

        if (in_rule && line[0] != '\t') {
            in_rule = false;
        }
    }

    fclose(fp);
    return has_error ? EXIT_FAILURE : EXIT_SUCCESS;
}

void print_help(const char *prog_name) {
    printf("Minimake - Lightweight Build Tool\n\n");
    printf("Usage: %s <command> [options]\n\n", prog_name);
    printf("Commands:\n");
    printf("  preprocess [-v|--verbose]  Clean and validate Makefile\n");
    printf("  check                      Syntax verification\n");
    printf("  build <target>             Execute build process\n\n");
    printf("Options:\n");
    printf("  -h, --help    Show this help message\n");
    printf("  -v, --verbose Enable verbose output\n");
}