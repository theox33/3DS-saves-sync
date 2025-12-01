#include "ui.h"
#include <3ds.h>
#include <stdio.h>
#include <stddef.h>

typedef struct {
    const char *label;
    int value;
} MenuOption;

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
    static const MenuOption options[] = {
        {"Scan & Upload saves", 1},
        {"Download & Restore saves", 2},
        {"Test connexion", 3},
        {"Edit config (TODO)", 4},
        {"Quitter", 0},
    };
    const size_t count = sizeof(options) / sizeof(options[0]);
    size_t selected = 0;
    bool redraw = true;
    const u32 up_mask = KEY_DUP | KEY_UP;
    const u32 down_mask = KEY_DDOWN | KEY_DOWN;

    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();

        if (kDown & up_mask) {
            selected = (selected == 0) ? count - 1 : selected - 1;
            redraw = true;
        } else if (kDown & down_mask) {
            selected = (selected + 1) % count;
            redraw = true;
        }

        if (kDown & KEY_A) {
            return options[selected].value;
        }

        if (kDown & (KEY_B | KEY_START)) {
            return 0;
        }

        if (redraw) {
            consoleClear();
            print_config(cfg);
            printf("=== 3DS Save Sync ===\n\n");
            for (size_t i = 0; i < count; ++i) {
                printf("%c %s\n", (i == selected) ? '>' : ' ', options[i].label);
            }
            printf("\nUtilise D-Pad pour naviguer, A pour valider, B/START pour quitter.\n");
            redraw = false;
            gfxFlushBuffers();
            gfxSwapBuffers();
        }

        gspWaitForVBlank();
    }

    return 0;
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
