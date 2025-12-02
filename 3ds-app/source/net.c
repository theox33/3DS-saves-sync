#include "net.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <3ds.h>   // ⬅ important pour httpc

// ===============================
//  Helpers HTTP bas niveau (impl httpc)
// ===============================

static bool http_get(
    const char *url,
    const char *api_key,
    int *out_status,
    char *out_body,
    size_t out_body_size
) {
    httpcContext ctx;
    Result res;
    u32 statuscode = 0;
    u32 downloaded = 0;

    if (!out_body || out_body_size == 0) {
        return false;
    }
    out_body[0] = '\0';

    printf("[HTTP] GET %s\n", url);

    res = httpcOpenContext(&ctx, HTTPC_METHOD_GET, url, 1);
    if (R_FAILED(res)) {
        printf("[HTTP] httpcOpenContext failed: 0x%08lX\n", res);
        return false;
    }

    // HTTPS: désactiver la vérification du certificat pour dev (à améliorer plus tard)
    httpcSetSSLOpt(&ctx, SSLCOPT_DisableVerify);

    // Header x-api-key
    if (api_key && api_key[0]) {
        res = httpcAddRequestHeaderField(&ctx, "x-api-key", api_key);
        if (R_FAILED(res)) {
            printf("[HTTP] httpcAddRequestHeaderField(x-api-key) failed: 0x%08lX\n", res);
            httpcCloseContext(&ctx);
            return false;
        }
    }

    res = httpcBeginRequest(&ctx);
    if (R_FAILED(res)) {
        printf("[HTTP] httpcBeginRequest failed: 0x%08lX\n", res);
        httpcCloseContext(&ctx);
        return false;
    }

    res = httpcGetResponseStatusCode(&ctx, &statuscode);
    if (R_FAILED(res)) {
        printf("[HTTP] httpcGetResponseStatusCode failed: 0x%08lX\n", res);
        httpcCloseContext(&ctx);
        return false;
    }

    if (out_status) *out_status = (int)statuscode;

    // Lecture de la réponse dans out_body
    u32 pos = 0;
    while (pos < out_body_size - 1) {
        u32 bytesRead = 0;
        res = httpcDownloadData(&ctx,
                                (u8 *)out_body + pos,
                                (u32)(out_body_size - 1 - pos),
                                &bytesRead);

        pos += bytesRead;

        if (R_FAILED(res)) {
            if (res == HTTPC_RESULTCODE_DOWNLOADPENDING)
                continue; // continuer jusqu'à la fin
            printf("[HTTP] httpcDownloadData failed: 0x%08lX\n", res);
            break;
        }

        if (bytesRead == 0) {
            break; // plus de données
        }
    }

    out_body[pos] = '\0';
    downloaded = pos;

    printf("[HTTP] status=%lu, downloaded=%lu bytes\n",
           (unsigned long)statuscode, (unsigned long)downloaded);

    httpcCloseContext(&ctx);
    return true;
}

static bool http_get_to_file(
    const char *url,
    const char *api_key,
    int *out_status,
    const char *local_path
) {
    httpcContext ctx;
    Result res;
    u32 statuscode = 0;

    printf("[HTTP] GET (to file) %s -> %s\n", url, local_path);

    FILE *f = fopen(local_path, "wb");
    if (!f) {
        printf("[HTTP] Impossible d'ouvrir le fichier en écriture: %s\n", local_path);
        return false;
    }

    res = httpcOpenContext(&ctx, HTTPC_METHOD_GET, url, 1);
    if (R_FAILED(res)) {
        printf("[HTTP] httpcOpenContext failed: 0x%08lX\n", res);
        fclose(f);
        return false;
    }

    httpcSetSSLOpt(&ctx, SSLCOPT_DisableVerify);

    if (api_key && api_key[0]) {
        res = httpcAddRequestHeaderField(&ctx, "x-api-key", api_key);
        if (R_FAILED(res)) {
            printf("[HTTP] httpcAddRequestHeaderField(x-api-key) failed: 0x%08lX\n", res);
            httpcCloseContext(&ctx);
            fclose(f);
            return false;
        }
    }

    res = httpcBeginRequest(&ctx);
    if (R_FAILED(res)) {
        printf("[HTTP] httpcBeginRequest failed: 0x%08lX\n", res);
        httpcCloseContext(&ctx);
        fclose(f);
        return false;
    }

    res = httpcGetResponseStatusCode(&ctx, &statuscode);
    if (R_FAILED(res)) {
        printf("[HTTP] httpcGetResponseStatusCode failed: 0x%08lX\n", res);
        httpcCloseContext(&ctx);
        fclose(f);
        return false;
    }

    if (out_status) *out_status = (int)statuscode;

    // Téléchargement en chunks vers le fichier
    u8 buffer[4096];
    while (1) {
        u32 bytesRead = 0;
        res = httpcDownloadData(&ctx, buffer, sizeof(buffer), &bytesRead);

        if (R_FAILED(res)) {
            if (res == HTTPC_RESULTCODE_DOWNLOADPENDING) {
                // continuer la boucle
                continue;
            }
            printf("[HTTP] httpcDownloadData failed: 0x%08lX\n", res);
            break;
        }

        if (bytesRead == 0) {
            // plus rien à lire
            break;
        }

        size_t written = fwrite(buffer, 1, bytesRead, f);
        if (written != bytesRead) {
            printf("[HTTP] fwrite incomplet\n");
            break;
        }
    }

    printf("[HTTP] status=%lu (download to file terminé)\n", (unsigned long)statuscode);

    httpcCloseContext(&ctx);
    fclose(f);

    return (statuscode >= 200 && statuscode < 300);
}

static bool http_post_multipart(
    const char *url,
    const char *api_key,
    const void *body,
    size_t body_size,
    int *out_status,
    char *out_body,
    size_t out_body_size,
    const char *boundary
) {
    httpcContext ctx;
    Result res;
    u32 statuscode = 0;

    if (!body || body_size == 0) return false;
    if (out_body && out_body_size > 0) out_body[0] = '\0';

    printf("[HTTP] POST multipart %s (body=%lu bytes)\n",
           url, (unsigned long)body_size);

    res = httpcOpenContext(&ctx, HTTPC_METHOD_POST, url, 1);
    if (R_FAILED(res)) {
        printf("[HTTP] httpcOpenContext failed: 0x%08lX\n", res);
        return false;
    }

    httpcSetSSLOpt(&ctx, SSLCOPT_DisableVerify);

    // Header x-api-key
    if (api_key && api_key[0]) {
        res = httpcAddRequestHeaderField(&ctx, "x-api-key", api_key);
        if (R_FAILED(res)) {
            printf("[HTTP] httpcAddRequestHeaderField(x-api-key) failed: 0x%08lX\n", res);
            httpcCloseContext(&ctx);
            return false;
        }
    }

    // Header Content-Type multipart/form-data
    char content_type[256];
    snprintf(content_type, sizeof(content_type),
             "multipart/form-data; boundary=%s", boundary);

    res = httpcAddRequestHeaderField(&ctx, "Content-Type", content_type);
    if (R_FAILED(res)) {
        printf("[HTTP] httpcAddRequestHeaderField(Content-Type) failed: 0x%08lX\n", res);
        httpcCloseContext(&ctx);
        return false;
    }

    // Données POST (multipart déjà construit en mémoire)
    res = httpcAddPostDataRaw(&ctx, (u32 *)body, (u32)body_size);
    if (R_FAILED(res)) {
        printf("[HTTP] httpcAddPostDataRaw failed: 0x%08lX\n", res);
        httpcCloseContext(&ctx);
        return false;
    }

    res = httpcBeginRequest(&ctx);
    if (R_FAILED(res)) {
        printf("[HTTP] httpcBeginRequest failed: 0x%08lX\n", res);
        httpcCloseContext(&ctx);
        return false;
    }

    res = httpcGetResponseStatusCode(&ctx, &statuscode);
    if (R_FAILED(res)) {
        printf("[HTTP] httpcGetResponseStatusCode failed: 0x%08lX\n", res);
        httpcCloseContext(&ctx);
        return false;
    }

    if (out_status) *out_status = (int)statuscode;

    // Lire la réponse (optionnel)
    if (out_body && out_body_size > 0) {
        u32 pos = 0;
        while (pos < out_body_size - 1) {
            u32 bytesRead = 0;
            res = httpcDownloadData(&ctx,
                                    (u8 *)out_body + pos,
                                    (u32)(out_body_size - 1 - pos),
                                    &bytesRead);

            pos += bytesRead;

            if (R_FAILED(res)) {
                if (res == HTTPC_RESULTCODE_DOWNLOADPENDING)
                    continue;
                printf("[HTTP] httpcDownloadData (POST) failed: 0x%08lX\n", res);
                break;
            }

            if (bytesRead == 0)
                break;
        }
        out_body[pos] = '\0';
    }

    printf("[HTTP] POST status=%lu\n", (unsigned long)statuscode);

    httpcCloseContext(&ctx);

    return (statuscode >= 200 && statuscode < 300);
}


// ===============================
//  Helpers internes
// ===============================

static void build_url(char *out, size_t out_size, const char *base, const char *suffix) {
    // base ex: "https://theo-avril.fr/api/3ds"
    // suffix ex: "/saves/health" ou "?gameId=..."
    snprintf(out, out_size, "%s%s", base, suffix);
}

static const char* get_filename_from_path(const char *path) {
    const char *slash = strrchr(path, '/');
    if (!slash) slash = strrchr(path, '\\');
    return slash ? slash + 1 : path;
}

// ===============================
//  Implémentation API haut niveau
// ===============================

bool net_test_connection(const AppConfig *cfg) {
    char url[512];
    char body[256];
    int status = 0;

    build_url(url, sizeof(url), cfg->api_url, "/saves/health");

    printf("[NET] Test connexion -> GET %s\n", url);

    if (!http_get(url, cfg->api_key, &status, body, sizeof(body))) {
        printf("[NET] http_get a échoué.\n");
        return false;
    }

    printf("[NET] HTTP status: %d, body: %s\n", status, body);

    if (status != 200) return false;

    // pour le moment on ne parse pas le JSON, on se contente du code HTTP
    return true;
}

bool net_list_saves(
    const AppConfig *cfg,
    const char *gameId,
    char *out_json,
    size_t out_size
) {
    char url[512];
    int status = 0;

    snprintf(url, sizeof(url), "%s/saves/list?gameId=%s", cfg->api_url, gameId);

    printf("[NET] List saves -> GET %s\n", url);

    if (!http_get(url, cfg->api_key, &status, out_json, out_size)) {
        printf("[NET] http_get a échoué.\n");
        return false;
    }

    printf("[NET] HTTP status: %d, body: %s\n", status, out_json);
    return (status == 200);
}

bool net_download_save(
    const AppConfig *cfg,
    const char *remote_path,
    const char *local_path
) {
    char url[1024];
    int status = 0;

    snprintf(url, sizeof(url), "%s/saves/download?path=%s", cfg->api_url, remote_path);

    printf("[NET] Download save -> GET %s\n", url);
    printf("[NET] Fichier local: %s\n", local_path);

    if (!http_get_to_file(url, cfg->api_key, &status, local_path)) {
        printf("[NET] http_get_to_file a échoué.\n");
        return false;
    }

    return (status == 200);
}

bool net_upload_save(
    const AppConfig *cfg,
    const char *local_path,
    const char *gameId,
    const char *slot
) {
    // 1) Lire le fichier .sav
    FILE *f = fopen(local_path, "rb");
    if (!f) {
        printf("[NET] Impossible d'ouvrir le fichier: %s\n", local_path);
        return false;
    }

    struct stat st;
    if (stat(local_path, &st) != 0) {
        printf("[NET] stat() a échoué pour %s\n", local_path);
        fclose(f);
        return false;
    }
    size_t file_size = (size_t)st.st_size;

    unsigned char *file_buf = (unsigned char *)malloc(file_size);
    if (!file_buf) {
        printf("[NET] malloc() a échoué pour file_buf\n");
        fclose(f);
        return false;
    }

    if (fread(file_buf, 1, file_size, f) != file_size) {
        printf("[NET] fread() incomplet\n");
        free(file_buf);
        fclose(f);
        return false;
    }
    fclose(f);

    // 2) Construire le body multipart/form-data (Option A)
    const char *boundary = "----3DSSaveSyncBoundary7E3F2A";
    const char *filename = get_filename_from_path(local_path);

    // Parties texte
    char *part_console = NULL;
    char *part_game = NULL;
    char *part_slot = NULL;
    char *part_file_header = NULL;
    char *part_file_footer = NULL;

    // consoleId
    {
        const char *tmpl =
            "--%s\r\n"
            "Content-Disposition: form-data; name=\"consoleId\"\r\n\r\n"
            "%s\r\n";
        size_t len = strlen(tmpl) + strlen(boundary) + strlen(cfg->console_id) + 1;
        part_console = (char *)malloc(len);
        if (!part_console) goto oom;
        snprintf(part_console, len, tmpl, boundary, cfg->console_id);
    }

    // gameId
    {
        const char *tmpl =
            "--%s\r\n"
            "Content-Disposition: form-data; name=\"gameId\"\r\n\r\n"
            "%s\r\n";
        size_t len = strlen(tmpl) + strlen(boundary) + strlen(gameId) + 1;
        part_game = (char *)malloc(len);
        if (!part_game) goto oom;
        snprintf(part_game, len, tmpl, boundary, gameId);
    }

    // slot
    {
        const char *tmpl =
            "--%s\r\n"
            "Content-Disposition: form-data; name=\"slot\"\r\n\r\n"
            "%s\r\n";
        size_t len = strlen(tmpl) + strlen(boundary) + strlen(slot) + 1;
        part_slot = (char *)malloc(len);
        if (!part_slot) goto oom;
        snprintf(part_slot, len, tmpl, boundary, slot);
    }

    // header de file
    {
        const char *tmpl =
            "--%s\r\n"
            "Content-Disposition: form-data; name=\"file\"; filename=\"%s\"\r\n"
            "Content-Type: application/octet-stream\r\n\r\n";
        size_t len = strlen(tmpl) + strlen(boundary) + strlen(filename) + 1;
        part_file_header = (char *)malloc(len);
        if (!part_file_header) goto oom;
        snprintf(part_file_header, len, tmpl, boundary, filename);
    }

    // footer (fin de multipart)
    {
        const char *tmpl = "\r\n--%s--\r\n";
        size_t len = strlen(tmpl) + strlen(boundary) + 1;
        part_file_footer = (char *)malloc(len);
        if (!part_file_footer) goto oom;
        snprintf(part_file_footer, len, tmpl, boundary);
    }

    // Taille totale du body
    size_t body_size =
        strlen(part_console) +
        strlen(part_game) +
        strlen(part_slot) +
        strlen(part_file_header) +
        file_size +
        strlen(part_file_footer);

    unsigned char *body = (unsigned char *)malloc(body_size);
    if (!body) {
        printf("[NET] malloc() a échoué pour body\n");
        goto oom;
    }

    // Assemblage du body
    size_t offset = 0;
    memcpy(body + offset, part_console, strlen(part_console));
    offset += strlen(part_console);

    memcpy(body + offset, part_game, strlen(part_game));
    offset += strlen(part_game);

    memcpy(body + offset, part_slot, strlen(part_slot));
    offset += strlen(part_slot);

    memcpy(body + offset, part_file_header, strlen(part_file_header));
    offset += strlen(part_file_header);

    memcpy(body + offset, file_buf, file_size);
    offset += file_size;

    memcpy(body + offset, part_file_footer, strlen(part_file_footer));
    offset += strlen(part_file_footer);

    if (offset != body_size) {
        printf("[NET] Erreur d'assemblage multipart: offset != body_size\n");
        free(body);
        goto oom;
    }

    // 3) Appel HTTP POST multipart
    char url[512];
    int status = 0;
    char resp[256] = {0};

    build_url(url, sizeof(url), cfg->api_url, "/saves/upload");

    printf("[NET] Upload save -> POST %s\n", url);
    printf("[NET] gameId=%s, slot=%s, consoleId=%s, file=%s (%lu bytes)\n",
           gameId, slot, cfg->console_id, filename, (unsigned long)file_size);

    bool ok = http_post_multipart(
        url,
        cfg->api_key,
        body,
        body_size,
        &status,
        resp,
        sizeof(resp),
        boundary
    );

    free(body);
    free(file_buf);
    free(part_console);
    free(part_game);
    free(part_slot);
    free(part_file_header);
    free(part_file_footer);

    if (!ok) {
        printf("[NET] http_post_multipart a échoué\n");
        return false;
    }

    printf("[NET] HTTP status: %d, body: %s\n", status, resp);

    return (status == 201 || status == 200);

oom:
    printf("[NET] OOM pendant la construction du multipart\n");
    if (file_buf) free(file_buf);
    if (part_console) free(part_console);
    if (part_game) free(part_game);
    if (part_slot) free(part_slot);
    if (part_file_header) free(part_file_header);
    if (part_file_footer) free(part_file_footer);
    return false;
}
