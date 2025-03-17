#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void print_help() {
    printf("Usage: minimake [OPTIONS] [TARGET]\n");
    printf("Options:\n");
    printf("  --help\tDisplay this help message\n");
    printf("  -v\t\tEnable verbose output\n");
}

int main(int argc, char *argv[]) {
    bool verbose = false;
    char *target = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_help();
            return 0;
        } else if (strcmp(argv[i], "-v") == 0) {
            verbose = true;
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "Error: Unknown option '%s'\n", argv[i]);
            return 1;
        } else {
            if (target != NULL) {
                fprintf(stderr, "Error: Multiple targets specified\n");
                return 1;
            }
            target = argv[i];
        }
    }
    return 0;
}