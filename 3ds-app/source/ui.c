#include "ui.h"
#include <3ds.h>
#include <stdio.h>

static void print_config(const AppConfig *cfg) {
    printf("=== Configuration NAS ===\n");
    printf("URL      : %s\n", cfg->nas_url);
    printf("Port     : %d\n", cfg->nas_port);
    printf("Protocol : %s\n", cfg->protocol);
    printf("Base path: %s\n", cfg->base_path);
    printf("User     : %s\n", cfg->username);
    printf("\n");
}

void ui_init(void) {
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
}

void ui_exit(void) {
    gfxExit();
}

int ui_main_menu(const AppConfig *cfg) {
    consoleClear();
    print_config(cfg);
    printf("=== 3DS Save Sync ===\n\n");
    printf("1) Scan & Upload saves\n");
    printf("2) Download & Restore saves\n");
    printf("3) Test connexion\n");
    printf("4) Edit config (TODO)\n");
    printf("0) Quitter\n\n");
    printf("Choix: ");

    int choice = -1;
    scanf("%d", &choice);

    return choice;
}

void ui_show_message(const char *msg) {
    consoleClear();
    printf("%s\n\n", msg);
}

void ui_wait_key(void) {
    printf("Appuie sur A pour continuer...\n");
    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown & KEY_A) break;
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
}
