#pragma once

#include <stddef.h>

#define MAX_SAVES 512

typedef struct {
    char path[512];
    char name[128];
} SaveEntry;

size_t saves_scan(SaveEntry *entries, size_t max_entries);
