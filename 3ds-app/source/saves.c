#include "saves.h"
#include "util.h"
#include <dirent.h>
#include <string.h>
#include <stdio.h>

static size_t scan_root(const char *root, SaveEntry *entries, size_t max_entries, size_t offset) {
    DIR *dir = opendir(root);
    if (!dir) return offset;

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL && offset < max_entries) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;

        SaveEntry *e = &entries[offset++];
        util_join_path(root, ent->d_name, e->path, sizeof(e->path));
        strncpy(e->name, ent->d_name, sizeof(e->name) - 1);
        e->name[sizeof(e->name) - 1] = '\0';
    }

    closedir(dir);
    return offset;
}

size_t saves_scan(SaveEntry *entries, size_t max_entries) {
    size_t count = 0;
    count = scan_root("sdmc:/3ds/Checkpoint/saves", entries, max_entries, count);
    count = scan_root("sdmc:/JKSV/Saves", entries, max_entries, count);
    count = scan_root("sdmc:/_nds/TWiLightMenu/saves", entries, max_entries, count);
    printf("Trouve %zu dossiers de sauvegarde.\n", count);
    return count;
}
