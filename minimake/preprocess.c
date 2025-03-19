#include "preprocess.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h> 

int preprocess_makefile(bool verbose) {
    FILE *fp = fopen("Makefile", "r");
    if (!fp) return EXIT_FAILURE;

    FILE *out = verbose ? fopen(CLEANED_FILE, "w") : NULL;
    char line[MAX_LINE_LEN];
    int processed = 0;

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        trim_trailing_spaces(line);
        if (is_blank_line(line)) continue;

        if (verbose && out) fprintf(out, "%s\n", line);
        processed++;
    }

    fclose(fp);
    if (verbose && out) fclose(out);
    return processed > 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}