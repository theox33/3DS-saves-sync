#include <3ds.h>
#include <stdio.h>
#include <stdbool.h>

#include "config.h"
#include "ui.h"
#include "sync.h"
#include "net.h"

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    // Initialisation du module HTTP (httpc)
    Result res = httpcInit(0);
    if (R_FAILED(res)) {
        // Si l'init HTTP échoue, on affiche un message simple en console
        gfxInitDefault();
        consoleInit(GFX_TOP, NULL);

        printf("httpcInit failed: 0x%08lX\n", res);
        printf("Impossible d'utiliser les fonctions reseau.\n");
        printf("Appuie sur START pour quitter.\n");

        while (aptMainLoop()) {
            hidScanInput();
            u32 kDown = hidKeysDown();
            if (kDown & KEY_START) break;

            gfxFlushBuffers();
            gfxSwapBuffers();
            gspWaitForVBlank();
        }

        gfxExit();
        return 1;
    }

    AppConfig cfg;
    bool loaded = config_load(&cfg, CONFIG_PATH);

    ui_init();

    if (!loaded) {
        ui_show_message("Config non trouvee. Valeurs par defaut chargees.");
        ui_wait_key();
        config_save(&cfg, CONFIG_PATH);
    }

    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START) break;

        int choice = ui_main_menu(&cfg);

        if (choice == 0) {
            break;
        } else if (choice == 1) {
            ui_show_message("Scan & Upload en cours...");
            SyncResult r = sync_upload_all(&cfg);
            printf("Resultat: %d\n", r);
            ui_wait_key();
        } else if (choice == 2) {
            ui_show_message("Download & Restore (TODO)...");
            SyncResult r = sync_download_all(&cfg);
            printf("Resultat: %d\n", r);
            ui_wait_key();
        } else if (choice == 3) {
            ui_show_message("Test de connexion...");
            bool ok = net_test_connection(&cfg);
            printf("Connexion: %s\n", ok ? "OK" : "ECHEC");
            ui_wait_key();
        } else if (choice == 4) {
            ui_show_message("Edition de config (TODO UI)...");
            ui_wait_key();
        }
    }

    ui_exit();
    httpcExit();  // Libère proprement le module HTTP

    return 0;
}
