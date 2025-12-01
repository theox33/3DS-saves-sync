#include "util.h"
#include <stdio.h>

void util_join_path(const char *a, const char *b, char *out, size_t out_size) {
    snprintf(out, out_size, "%s/%s", a, b);
}
