#include "sync.h"
#include "saves.h"
#include "net.h"
#include <stdio.h>
#include <string.h>

static void derive_game_id(const SaveEntry *entry, char *out, size_t out_size) {
    // Pour un premier jet : on utilise le nom du dossier/fichier comme gameId
    // ex: "pokemon-black" si entry->name = "pokemon-black"
    strncpy(out, entry->name, out_size - 1);
    out[out_size - 1] = '\0';
}

SyncResult sync_upload_all(const AppConfig *cfg) {
    SaveEntry entries[MAX_SAVES];
    size_t count = saves_scan(entries, MAX_SAVES);

    if (count == 0) {
        printf("[SYNC] Aucune sauvegarde trouvée.\n");
        return SYNC_OK;
    }

    printf("[SYNC] %zu sauvegardes trouvées. Début upload...\n", count);

    for (size_t i = 0; i < count; ++i) {
        char gameId[128];
        derive_game_id(&entries[i], gameId, sizeof(gameId));

        const char *slot = "slot1"; // pour l'instant on fixe slot1

        printf("[SYNC] Upload %s (gameId=%s, slot=%s)\n",
               entries[i].path, gameId, slot);

        if (!net_upload_save(cfg, entries[i].path, gameId, slot)) {
            printf("[SYNC] Echec upload pour %s\n", entries[i].path);
            return SYNC_ERR_NET;
        }
    }

    printf("[SYNC] Upload terminé pour %zu sauvegardes.\n", count);
    return SYNC_OK;
}

SyncResult sync_download_all(const AppConfig *cfg) {
    // Pour l'instant, on ne gère pas encore l'interaction :
    // - dans une V1 : tu pourras demander gameId à l'utilisateur,
    //   appeler net_list_saves, parser le JSON, etc.
    printf("[SYNC] TODO: implémenter le flux de download interactif.\n");
    return SYNC_OK;
}
