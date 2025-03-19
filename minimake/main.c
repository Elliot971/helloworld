#include "minimake.h"
#include "preprocess.h"
#include "check_syntax.h"
#include "build.h"

void print_help(const char *prog_name) {
    printf("Usage: %s <command> [options]\n", prog_name);
    printf("Commands:\n");
    printf("  preprocess [-v]  Clean Makefile\n");
    printf("  check            Syntax check\n");
    printf("  build <target>   Build target\n");
}

int main(int argc, char **argv) {
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        print_help(argv[0]);
        return EXIT_SUCCESS;
    }

    const char *subcmd = argv[1];
    
    if (strcmp(subcmd, "preprocess") == 0) {
        bool verbose = (argc > 2 && strcmp(argv[2], "-v") == 0);
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
        return build_target(argv[2]);
    }
    else {
        fprintf(stderr, "Error: Invalid command\n");
        return EXIT_FAILURE;
    }
}