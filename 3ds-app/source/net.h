#pragma once

#include "config.h"
#include <stdbool.h>
#include <stddef.h>

// Test GET /saves/health
bool net_test_connection(const AppConfig *cfg);

// Upload d'un fichier .sav via multipart/form-data vers POST /saves/upload
bool net_upload_save(
    const AppConfig *cfg,
    const char *local_path,
    const char *gameId,
    const char *slot
);

// Liste les saves (GET /saves/list?gameId=...)
bool net_list_saves(
    const AppConfig *cfg,
    const char *gameId,
    char *out_json,
    size_t out_size
);

// Télécharge une save (GET /saves/download?path=...)
bool net_download_save(
    const AppConfig *cfg,
    const char *remote_path,
    const char *local_path
);
