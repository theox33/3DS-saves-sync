#include "net.h"
#include <stdio.h>

bool net_test_connection(const AppConfig *cfg) {
    printf("[NET] TODO: tester la connexion vers %s:%d (%s)\n",
           cfg->nas_url, cfg->nas_port, cfg->protocol);
    return false;
}

bool net_upload_archive(const AppConfig *cfg, const char *local_path, const char *remote_name) {
    printf("[NET] TODO: upload %s -> %s/%s\n",
           local_path, cfg->base_path, remote_name);
    return false;
}

bool net_download_archive(const AppConfig *cfg, const char *remote_name, const char *local_path) {
    printf("[NET] TODO: download %s/%s -> %s\n",
           cfg->base_path, remote_name, local_path);
    return false;
}
