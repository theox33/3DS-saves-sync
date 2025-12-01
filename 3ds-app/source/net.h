#pragma once

#include "config.h"
#include "saves.h"
#include <stdbool.h>

bool net_test_connection(const AppConfig *cfg);
bool net_upload_archive(const AppConfig *cfg, const char *local_path, const char *remote_name);
bool net_download_archive(const AppConfig *cfg, const char *remote_name, const char *local_path);
