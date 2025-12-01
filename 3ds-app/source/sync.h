#pragma once

#include "config.h"

typedef enum {
    SYNC_OK = 0,
    SYNC_ERR_NET,
    SYNC_ERR_FS,
    SYNC_ERR_CONFIG,
} SyncResult;

SyncResult sync_upload_all(const AppConfig *cfg);
SyncResult sync_download_all(const AppConfig *cfg);
