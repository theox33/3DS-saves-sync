#include "config.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static void trim_newline(char *s) {
    char *p = strchr(s, '\n');
    if (p) *p = '\0';
}

void config_set_defaults(AppConfig *cfg) {
    memset(cfg, 0, sizeof(*cfg));
    strcpy(cfg->api_url, "https://theo-avril.fr/api/3ds");
    strcpy(cfg->api_key, "CHANGE_ME_API_KEY");
    strcpy(cfg->console_id, "3ds-default");
}

static void parse_string_field(const char *key, const char *line, char *out, size_t out_size) {
    const char *p = strstr(line, key);
    if (!p) return;
    p = strchr(p, ':');
    if (!p) return;
    p++;
    while (*p == ' ' || *p == '\"') p++;
    const char *end = strchr(p, '\"');
    if (!end) return;
    size_t len = (size_t)(end - p);
    if (len >= out_size) len = out_size - 1;
    memcpy(out, p, len);
    out[len] = '\0';
}

bool config_load(AppConfig *cfg, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        // pas de fichier -> valeurs par défaut
        config_set_defaults(cfg);
        return false;
    }

    config_set_defaults(cfg);

    char line[512];
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        parse_string_field("api_url", line, cfg->api_url, sizeof(cfg->api_url));
        parse_string_field("api_key", line, cfg->api_key, sizeof(cfg->api_key));
        parse_string_field("console_id", line, cfg->console_id, sizeof(cfg->console_id));
    }

    fclose(f);
    return true;
}

bool config_save(const AppConfig *cfg, const char *path) {
    FILE *f = fopen(path, "w");
    if (!f) return false;

    fprintf(f,
            "{\n"
            "  \"api_url\": \"%s\",\n"
            "  \"api_key\": \"%s\",\n"
            "  \"console_id\": \"%s\"\n"
            "}\n",
            cfg->api_url,
            cfg->api_key,
            cfg->console_id
    );

    fclose(f);
    return true;
}
