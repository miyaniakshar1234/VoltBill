/**
 * @file ui.c
 * @brief Implementation of terminal user interface and interactive widgets.
 * @author Akshar Miyani (MCA 1st Sem, Manipal University Jaipur)
 */

#include "ui.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void ui_header(const char *title, const char *subtitle) {
    clear_screen();
    printf("\n");
    printf("  " CLR_CYAN CLR_BOLD "VOLTBILL" CLR_RESET " " CLR_GRAY "v1.0.0" CLR_RESET "  " 
           CLR_DARK_GRAY "│" CLR_RESET "  " CLR_VIOLET "%s" CLR_RESET "\n", title ? title : "");
    if (subtitle && subtitle[0] != '\0') {
        printf("  " CLR_GRAY "%s" CLR_RESET "\n", subtitle);
    }
    printf("  " CLR_DARK_GRAY);
    for (int i = 0; i < 76; i++) printf("─");
    printf(CLR_RESET "\n\n");
}

void ui_box_top(int width, const char *title) {
    printf("  " DBOX_TL);
    if (title && title[0] != '\0') {
        printf(DBOX_H " [ " CLR_CYAN CLR_BOLD "%s" CLR_RESET " ] ", title);
        int title_len = (int)strlen(title) + 6;
        for (int i = 0; i < width - title_len; i++) printf(DBOX_H);
    } else {
        for (int i = 0; i < width; i++) printf(DBOX_H);
    }
    printf(DBOX_TR "\n");
}

void ui_box_row(int width, const char *label, const char *value) {
    int label_len = label ? (int)strlen(label) : 0;
    int val_len = value ? (int)strlen(value) : 0;
    int pad = width - (label_len + val_len + 4);
    if (pad < 0) pad = 0;

    printf("  " DBOX_V "  " CLR_WHITE "%s" CLR_RESET, label ? label : "");
    for (int i = 0; i < pad; i++) printf(" ");
    printf(CLR_YELLOW CLR_BOLD "%s" CLR_RESET "  " DBOX_V "\n", value ? value : "");
}

void ui_box_divider(int width) {
    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < width; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");
}

void ui_box_bottom(int width) {
    printf("  " DBOX_BL);
    for (int i = 0; i < width; i++) printf(DBOX_H);
    printf(DBOX_BR "\n");
}

void ui_status_bar(const char *left_text, const char *right_text) {
    printf("\n  " CLR_DARK_GRAY);
    for (int i = 0; i < 76; i++) printf("─");
    printf(CLR_RESET "\n");
    printf("  " CLR_GRAY "%-45s" CLR_RESET " " CLR_VIOLET "%29s" CLR_RESET "\n",
           left_text ? left_text : "VoltBill Utility Engine",
           right_text ? right_text : "Akshar Miyani (MCA, MUJ)");
}

void ui_badge(const char *label, const char *color) {
    printf("%s[%s]" CLR_RESET, color ? color : CLR_CYAN, label ? label : "");
}

void ui_progress_bar(double percent, int bar_width) {
    if (percent < 0.0) percent = 0.0;
    if (percent > 100.0) percent = 100.0;

    int filled = (int)((percent / 100.0) * bar_width);
    printf("[");
    printf(CLR_CYAN);
    for (int i = 0; i < filled; i++) printf("█");
    printf(CLR_DARK_GRAY);
    for (int i = filled; i < bar_width; i++) printf("░");
    printf(CLR_RESET "] " CLR_YELLOW "%5.1f%%" CLR_RESET, percent);
}

int ui_menu(const char *title, const char **options, int count, int default_idx) {
    if (count <= 0 || !options) return -1;
    int current = (default_idx >= 0 && default_idx < count) ? default_idx : 0;

    while (1) {
        /* Save cursor or redraw menu block */
        ui_header(title, "Use Arrow Keys [↑/↓] or [1-9] to navigate, [Enter] to select, [Esc] to return");

        printf("  " CLR_GRAY "┌────────────────────────────────────────────────────────────────────────┐" CLR_RESET "\n");
        for (int i = 0; i < count; i++) {
            if (i == current) {
                printf("  " CLR_GRAY "│ " CLR_RESET BG_CYAN CLR_WHITE CLR_BOLD "  ► [%d] %-61s " CLR_RESET CLR_GRAY "│" CLR_RESET "\n", 
                       i + 1, options[i]);
            } else {
                printf("  " CLR_GRAY "│ " CLR_RESET "    [%d] " CLR_WHITE "%-61s" CLR_RESET CLR_GRAY "│" CLR_RESET "\n", 
                       i + 1, options[i]);
            }
        }
        printf("  " CLR_GRAY "└────────────────────────────────────────────────────────────────────────┘" CLR_RESET "\n");

        ui_status_bar("Navigate: ↑ / ↓  |  Select: Enter  |  Exit: Esc", "Akshar Miyani • MUJ");

        int key = read_key();
        if (key == KEY_UP) {
            current = (current - 1 + count) % count;
        } else if (key == KEY_DOWN) {
            current = (current + 1) % count;
        } else if (key == KEY_ENTER) {
            return current;
        } else if (key == KEY_ESC) {
            return -1; /* Cancel / Return */
        } else if (key >= '1' && key <= '9') {
            int num = key - '1';
            if (num < count) {
                return num;
            }
        } else if (key == 'q' || key == 'Q') {
            return -1;
        }
    }
}

void ui_message_box(const char *title, const char *message, int is_success) {
    const char *color = is_success ? CLR_GREEN : CLR_RED;
    const char *icon = is_success ? "✓" : "✗";

    printf("\n  " CLR_DARK_GRAY "┌────────────────────────────────────────────────────────────────────────┐" CLR_RESET "\n");
    printf("  " CLR_DARK_GRAY "│ " CLR_RESET "%s%s %-68s" CLR_RESET CLR_DARK_GRAY "│" CLR_RESET "\n", color, icon, title ? title : "Notification");
    printf("  " CLR_DARK_GRAY "│ " CLR_RESET "  " CLR_WHITE "%-66s" CLR_RESET CLR_DARK_GRAY "│" CLR_RESET "\n", message ? message : "");
    printf("  " CLR_DARK_GRAY "└────────────────────────────────────────────────────────────────────────┘" CLR_RESET "\n");
    pause_prompt();
}
