#include "preprocess.h"
#include "utils.h"

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
        line[strcspn(line, "\n")] = '\0';
        char *comment = strchr(line, '#');
        if (comment) *comment = '\0';
        trim_trailing_spaces(line);

        if (!is_blank_line(line)) {
            if (verbose) {
                if (out) fprintf(out, "%s\n", line);
                printf("[DEBUG] %s\n", line);
            }
            processed++;
        }
    }

    fclose(fp);
    if (verbose && out) fclose(out);
    return processed > 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}