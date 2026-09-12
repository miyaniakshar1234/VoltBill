/**
 * @file ui.c
 * @brief High-impact Cyberpunk / Volt aesthetic terminal UI engine.
 * @author Akshar Miyani
 * @version 2.0.0
 */

#include "ui.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void ui_print_gradient(const char *text, int r1, int g1, int b1, int r2, int g2, int b2, int bold) {
    if (!text) return;
    int len = (int)strlen(text);
    if (len == 0) return;

    if (bold) printf(CLR_BOLD);

    for (int i = 0; i < len; i++) {
        float factor = (len > 1) ? ((float)i / (float)(len - 1)) : 0.0f;
        int r = (int)(r1 + (r2 - r1) * factor);
        int g = (int)(g1 + (g2 - g1) * factor);
        int b = (int)(b1 + (b2 - b1) * factor);

        printf("\033[38;2;%d;%d;%dm%c", r, g, b, text[i]);
    }
    printf(CLR_RESET);
}

void ui_telemetry_hud(void) {
    printf("  " CLR_DARK_GRAY "╭─[ " CLR_CYAN CLR_BOLD "VOLTBILL CORE v2.0" CLR_RESET CLR_DARK_GRAY " ]"
           "──[ " CLR_GREEN "STATUS: OPTIMAL 100%%" CLR_RESET CLR_DARK_GRAY " ]"
           "──[ " CLR_YELLOW "FREQ: 50.02 Hz" CLR_RESET CLR_DARK_GRAY " ]"
           "──[ " CLR_VIOLET "ENGINEER: AKSHAR MIYANI" CLR_RESET CLR_DARK_GRAY " ]─╮" CLR_RESET "\n");
    printf("  " CLR_DARK_GRAY "╰" CLR_RESET);
    for (int i = 0; i < 78; i++) printf(CLR_DARK_GRAY "─" CLR_RESET);
    printf(CLR_DARK_GRAY "╯" CLR_RESET "\n");
}

void ui_header(const char *title, const char *subtitle) {
    clear_screen();
    printf("\n");
    ui_telemetry_hud();
    printf("\n  ");
    ui_print_gradient(title ? title : "VOLTBILL UTILITY ENGINE", 0, 240, 255, 189, 0, 255, 1);
    printf("\n");
    if (subtitle && subtitle[0] != '\0') {
        printf("  " CLR_GRAY "◈ %s" CLR_RESET "\n", subtitle);
    }
    printf("  " CLR_DARK_GRAY);
    for (int i = 0; i < 78; i++) printf("═");
    printf(CLR_RESET "\n\n");
}

void ui_box_top(int width, const char *title) {
    printf("  " DBOX_TL);
    if (title && title[0] != '\0') {
        printf(DBOX_H " [ ");
        ui_print_gradient(title, 0, 240, 255, 255, 230, 0, 1);
        printf(" ] ");
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
    for (int i = 0; i < 78; i++) printf("─");
    printf(CLR_RESET "\n");
    printf("  " CLR_GRAY "%-52s" CLR_RESET " " CLR_CYAN CLR_BOLD "%24s" CLR_RESET "\n",
           left_text ? left_text : "VoltBill Power Utility Engine",
           right_text ? right_text : "AKSHAR MIYANI");
}

void ui_pill_badge(const char *label, const char *color) {
    printf("%s(● %s)" CLR_RESET, color ? color : CLR_CYAN, label ? label : "");
}

void ui_progress_bar(double percent, int bar_width) {
    if (percent < 0.0) percent = 0.0;
    if (percent > 100.0) percent = 100.0;

    int filled = (int)((percent / 100.0) * bar_width);
    printf("[");
    printf(CLR_CYAN);
    for (int i = 0; i < filled; i++) printf("▰");
    printf(CLR_DARK_GRAY);
    for (int i = filled; i < bar_width; i++) printf("▱");
    printf(CLR_RESET "] " CLR_YELLOW "%5.1f%%" CLR_RESET, percent);
}

void ui_gauge_bar(double current, double max_val, int width, const char *unit) {
    if (max_val <= 0.0) max_val = 1.0;
    double ratio = current / max_val;
    if (ratio > 1.0) ratio = 1.0;
    if (ratio < 0.0) ratio = 0.0;

    int filled = (int)(ratio * width);

    /* Dynamic color based on utilization */
    const char *color = CLR_GREEN;
    if (ratio > 0.85) color = CLR_RED;
    else if (ratio > 0.65) color = CLR_YELLOW;

    printf("[");
    printf("%s", color);
    for (int i = 0; i < filled; i++) printf("▰");
    printf(CLR_DARK_GRAY);
    for (int i = filled; i < width; i++) printf("▱");
    printf(CLR_RESET "] %s%5.1f%%%s (" CLR_WHITE "%.1f / %.1f %s" CLR_RESET ")",
           color, ratio * 100.0, CLR_RESET, current, max_val, unit ? unit : "");
}

int ui_menu(const char *title, const char **options, int count, int default_idx) {
    if (count <= 0 || !options) return -1;
    int current = (default_idx >= 0 && default_idx < count) ? default_idx : 0;

    while (1) {
        ui_header(title, "Use Arrow Keys [↑/↓] or [1-9] to Navigate  ◈  [Enter] Select  ◈  [Esc] Back");

        printf("  " CLR_DARK_GRAY "╭────────────────────────────────────────────────────────────────────────────╮" CLR_RESET "\n");
        for (int i = 0; i < count; i++) {
            if (i == current) {
                printf("  " CLR_DARK_GRAY "│ " CLR_RESET BG_CYAN CLR_WHITE CLR_BOLD "  ⚡  [%d]  %-58s " CLR_RESET CLR_DARK_GRAY "│" CLR_RESET "\n", 
                       i + 1, options[i]);
            } else {
                printf("  " CLR_DARK_GRAY "│ " CLR_RESET "      [%d]  " CLR_WHITE "%-58s" CLR_RESET CLR_DARK_GRAY "│" CLR_RESET "\n", 
                       i + 1, options[i]);
            }
        }
        printf("  " CLR_DARK_GRAY "╰────────────────────────────────────────────────────────────────────────────╯" CLR_RESET "\n");

        ui_status_bar("Navigate: ↑ / ↓  |  Select: Enter  |  Exit: Esc", "Akshar Miyani");

        int key = read_key();
        if (key == KEY_UP) {
            current = (current - 1 + count) % count;
        } else if (key == KEY_DOWN) {
            current = (current + 1) % count;
        } else if (key == KEY_ENTER) {
            return current;
        } else if (key == KEY_ESC) {
            return -1;
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

    printf("\n  " CLR_DARK_GRAY "╭────────────────────────────────────────────────────────────────────────────╮" CLR_RESET "\n");
    printf("  " CLR_DARK_GRAY "│ " CLR_RESET "%s%s %-70s" CLR_RESET CLR_DARK_GRAY "│" CLR_RESET "\n", color, icon, title ? title : "Notification");
    printf("  " CLR_DARK_GRAY "│ " CLR_RESET "  " CLR_WHITE "%-68s" CLR_RESET CLR_DARK_GRAY "│" CLR_RESET "\n", message ? message : "");
    printf("  " CLR_DARK_GRAY "╰────────────────────────────────────────────────────────────────────────────╯" CLR_RESET "\n");
    pause_prompt();
}

void ui_render_ascii_qr(const char *payload) {
    printf("      " CLR_WHITE "█████████████████████████████████" CLR_RESET "\n");
    printf("      " CLR_WHITE "██ " CLR_DARK_GRAY "███████" CLR_WHITE " " CLR_CYAN "█▀▄" CLR_WHITE " " CLR_DARK_GRAY "███████" CLR_WHITE " ██" CLR_RESET "\n");
    printf("      " CLR_WHITE "██ " CLR_DARK_GRAY "█     █" CLR_WHITE " " CLR_CYAN "▄ █" CLR_WHITE " " CLR_DARK_GRAY "█     █" CLR_WHITE " ██" CLR_RESET "\n");
    printf("      " CLR_WHITE "██ " CLR_DARK_GRAY "█ ███ █" CLR_WHITE " " CLR_YELLOW "██▀" CLR_WHITE " " CLR_DARK_GRAY "█ ███ █" CLR_WHITE " ██" CLR_RESET "\n");
    printf("      " CLR_WHITE "██ " CLR_DARK_GRAY "█ ███ █" CLR_WHITE " " CLR_YELLOW "▄▀▄" CLR_WHITE " " CLR_DARK_GRAY "█ ███ █" CLR_WHITE " ██" CLR_RESET "\n");
    printf("      " CLR_WHITE "██ " CLR_DARK_GRAY "█     █" CLR_WHITE " " CLR_CYAN "█▄█" CLR_WHITE " " CLR_DARK_GRAY "█     █" CLR_WHITE " ██" CLR_RESET "\n");
    printf("      " CLR_WHITE "██ " CLR_DARK_GRAY "███████" CLR_WHITE " " CLR_CYAN "█ █" CLR_WHITE " " CLR_DARK_GRAY "███████" CLR_WHITE " ██" CLR_RESET "\n");
    printf("      " CLR_WHITE "██         " CLR_CYAN "▄▀█▀▄" CLR_WHITE "         ██" CLR_RESET "\n");
    printf("      " CLR_WHITE "██ " CLR_CYAN "██▀▄█▄▀▄█▀▄█▄▀█▄█▀▄█▄█" CLR_WHITE " ██" CLR_RESET "\n");
    printf("      " CLR_WHITE "██ " CLR_CYAN "█▄ ▄█▀▄ ▀ ▄█▀ █▄▀ █▄ ▀" CLR_WHITE " ██" CLR_RESET "\n");
    printf("      " CLR_WHITE "██ " CLR_DARK_GRAY "███████" CLR_WHITE " " CLR_CYAN "█ ▀▄█ ▄▀█▄█" CLR_WHITE " ██" CLR_RESET "\n");
    printf("      " CLR_WHITE "██ " CLR_DARK_GRAY "█     █" CLR_WHITE " " CLR_YELLOW "▄█▀█▀▄ █▀▄█" CLR_WHITE " ██" CLR_RESET "\n");
    printf("      " CLR_WHITE "██ " CLR_DARK_GRAY "█ ███ █" CLR_WHITE " " CLR_YELLOW "██▀ █▀█ █ █" CLR_WHITE " ██" CLR_RESET "\n");
    printf("      " CLR_WHITE "██ " CLR_DARK_GRAY "█     █" CLR_WHITE " " CLR_CYAN "▀▄▀ ▄▀█ ▄ █" CLR_WHITE " ██" CLR_RESET "\n");
    printf("      " CLR_WHITE "██ " CLR_DARK_GRAY "███████" CLR_WHITE " " CLR_CYAN "█▄█▀ █▄▀█▀█" CLR_WHITE " ██" CLR_RESET "\n");
    printf("      " CLR_WHITE "█████████████████████████████████" CLR_RESET "\n");
    if (payload && payload[0] != '\0') {
        printf("      " CLR_GRAY "UPI PAY: " CLR_CYAN "%s" CLR_RESET "\n", payload);
    }
}
