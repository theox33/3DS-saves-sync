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
    strcpy(cfg->nas_url, "https://example-nas.local");
    cfg->nas_port = 443;
    strcpy(cfg->protocol, "https");
    strcpy(cfg->base_path, "/saves/3ds/");
    strcpy(cfg->username, "user");
    strcpy(cfg->password, "password");
}

static void parse_string_field(const char *key, const char *line, char *out, size_t out_size) {
    const char *p = strstr(line, key);
    if (!p) return;
    p = strchr(p, ':');
    if (!p) return;
    p++;
    while (*p == ' ' || *p == '"') p++;
    const char *end = strchr(p, '"');
    if (!end) return;
    size_t len = (size_t)(end - p);
    if (len >= out_size) len = out_size - 1;
    memcpy(out, p, len);
    out[len] = '\0';
}

static void parse_int_field(const char *key, const char *line, int *out) {
    const char *p = strstr(line, key);
    if (!p) return;
    p = strchr(p, ':');
    if (!p) return;
    p++;
    *out = atoi(p);
}

bool config_load(AppConfig *cfg, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        config_set_defaults(cfg);
        return false;
    }

    config_set_defaults(cfg);

    char line[512];
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        parse_string_field("nas_url", line, cfg->nas_url, sizeof(cfg->nas_url));
        parse_string_field("protocol", line, cfg->protocol, sizeof(cfg->protocol));
        parse_string_field("base_path", line, cfg->base_path, sizeof(cfg->base_path));
        parse_string_field("username", line, cfg->username, sizeof(cfg->username));
        parse_string_field("password", line, cfg->password, sizeof(cfg->password));
        parse_int_field("nas_port", line, &cfg->nas_port);
    }

    fclose(f);
    return true;
}

bool config_save(const AppConfig *cfg, const char *path) {
    FILE *f = fopen(path, "w");
    if (!f) return false;

    fprintf(f,
            "{\n"
            "  \"nas_url\": \"%s\",\n"
            "  \"nas_port\": %d,\n"
            "  \"protocol\": \"%s\",\n"
            "  \"base_path\": \"%s\",\n"
            "  \"username\": \"%s\",\n"
            "  \"password\": \"%s\"\n"
            "}\n",
            cfg->nas_url,
            cfg->nas_port,
            cfg->protocol,
            cfg->base_path,
            cfg->username,
            cfg->password);

    fclose(f);
    return true;
}
