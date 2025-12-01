#pragma once

#include <stdbool.h>

#define CONFIG_PATH "sdmc:/3ds-save-sync/config.json"

typedef struct {
    char nas_url[256];
    int  nas_port;
    char protocol[8];
    char base_path[256];
    char username[64];
    char password[64];
} AppConfig;

bool config_load(AppConfig *cfg, const char *path);
bool config_save(const AppConfig *cfg, const char *path);
void config_set_defaults(AppConfig *cfg);
