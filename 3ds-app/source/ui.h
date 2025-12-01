#pragma once

#include "config.h"

void ui_init(void);
void ui_exit(void);

int ui_main_menu(const AppConfig *cfg);

void ui_show_message(const char *msg);
void ui_wait_key(void);
