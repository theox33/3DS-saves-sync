#include <3ds.h>
#include <stdio.h>
#include <stdbool.h>

#include "config.h"
#include "ui.h"
#include "sync.h"
#include "net.h"

int main(int argc, char **argv) {
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
    return 0;
}
