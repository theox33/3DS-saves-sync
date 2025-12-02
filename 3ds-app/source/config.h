#pragma once

#include <stdbool.h>

#define CONFIG_PATH "sdmc:/3ds-save-sync/config.json"

typedef struct {
    char api_url[256];     // ex: "https://theo-avril.fr/api/3ds"
    char api_key[128];     // header x-api-key
    char console_id[64];   // ex: "3ds-theo-001"
} AppConfig;

bool config_load(AppConfig *cfg, const char *path);
bool config_save(const AppConfig *cfg, const char *path);
void config_set_defaults(AppConfig *cfg);
