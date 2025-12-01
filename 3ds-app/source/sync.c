#include "sync.h"
#include "saves.h"
#include "net.h"
#include "util.h"
#include <stdio.h>

SyncResult sync_upload_all(const AppConfig *cfg) {
    SaveEntry entries[MAX_SAVES];
    size_t count = saves_scan(entries, MAX_SAVES);

    if (count == 0) {
        printf("[SYNC] Aucune sauvegarde trouvée.\n");
        return SYNC_OK;
    }

    for (size_t i = 0; i < count; ++i) {
        char archive_path[512];
        char archive_name[256];

        util_join_path("sdmc:/3ds-save-sync/tmp", entries[i].name, archive_path, sizeof(archive_path));
        snprintf(archive_name, sizeof(archive_name), "%s.zip", entries[i].name);

        printf("[SYNC] TODO: compresser %s -> %s\n", entries[i].path, archive_path);

        if (!net_upload_archive(cfg, archive_path, archive_name)) {
            printf("[SYNC] Echec upload %s\n", entries[i].name);
            return SYNC_ERR_NET;
        }
    }

    return SYNC_OK;
}

SyncResult sync_download_all(const AppConfig *cfg) {
    printf("[SYNC] TODO: implémenter la liste des saves distantes + restauration.\n");
    return SYNC_OK;
}
