#pragma once

#include "config.h"

typedef enum {
    SYNC_OK = 0,
    SYNC_ERR_NET,
    SYNC_ERR_FS,
    SYNC_ERR_CONFIG,
} SyncResult;

// Upload simple : on envoie chaque save trouvée comme un slot1
SyncResult sync_upload_all(const AppConfig *cfg);

// TODO: implémenter plus tard un download interactif
SyncResult sync_download_all(const AppConfig *cfg);
