/**
 * @file ui.c
 * @brief High-impact Cyberpunk / Volt aesthetic terminal UI engine.
 * @author Akshar Miyani
 * @version 2.0.0
 */

#include "ui.h"
#include "utils.h"
#include "qrcodegen.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

int ui_visual_len(const char *s) {
    if (!s) return 0;
    int len = 0;
    const unsigned char *p = (const unsigned char *)s;
    while (*p) {
        if (*p == '\033') {
            p++;
            if (*p == '[') {
                p++;
                while (*p && *p != 'm' && *p != 'H' && *p != 'J' && *p != 'K') {
                    p++;
                }
                if (*p) p++;
            }
        } else if (*p < 0x80) {
            if (*p >= 32 && *p < 127) {
                len++;
            }
            p++;
        } else if ((*p & 0xF8) == 0xF0) {
            len += 2;
            p += 4;
        } else if ((*p & 0xF0) == 0xE0) {
            len += 1;
            p += 3;
        } else if ((*p & 0xE0) == 0xC0) {
            len += 1;
            p += 2;
        } else {
            p++;
        }
    }
    return len;
}

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

void ui_card_begin(int width, const char *title) {
    if (width <= 10) width = 76;
    printf("  " DBOX_TL);
    if (title && title[0] != '\0') {
        int t_len = ui_visual_len(title);
        int rem = width - (t_len + 6);
        if (rem < 0) rem = 0;
        printf(DBOX_H " [ ");
        ui_print_gradient(title, 0, 240, 255, 255, 230, 0, 1);
        printf(" ] ");
        for (int i = 0; i < rem; i++) printf(DBOX_H);
    } else {
        for (int i = 0; i < width; i++) printf(DBOX_H);
    }
    printf(DBOX_TR "\n");
}

void ui_card_text(int width, const char *fmt, ...) {
    if (width <= 10) width = 76;
    char buffer[2048];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    int vis = ui_visual_len(buffer);
    int pad = width - 2 - vis;
    if (pad < 0) pad = 0;

    printf("  " DBOX_V "  %s%*s" DBOX_V "\n", buffer, pad, "");
}

void ui_card_row(int width, const char *left_text, const char *right_text) {
    if (width <= 10) width = 76;
    const char *l = left_text ? left_text : "";
    const char *r = right_text ? right_text : "";

    int l_vis = ui_visual_len(l);
    int r_vis = ui_visual_len(r);

    int pad = width - 4 - l_vis - r_vis;
    if (pad < 0) pad = 0;

    printf("  " DBOX_V "  %s%*s%s  " DBOX_V "\n", l, pad, "", r);
}

void ui_card_section(int width, const char *title) {
    if (width <= 10) width = 76;
    char buffer[256];
    snprintf(buffer, sizeof(buffer), CLR_CYAN CLR_BOLD "%s" CLR_RESET, title ? title : "");
    ui_card_text(width, buffer);
}

void ui_card_divider(int width) {
    if (width <= 10) width = 76;
    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < width; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");
}

void ui_card_end(int width) {
    if (width <= 10) width = 76;
    printf("  " DBOX_BL);
    for (int i = 0; i < width; i++) printf(DBOX_H);
    printf(DBOX_BR "\n");
}

void ui_card_qr(int width, const char *payload) {
    if (width <= 10) width = 76;
    if (!payload || strlen(payload) == 0) {
        payload = "upi://pay?pa=voltbill.utility@axisbank&pn=VoltBill%20Utility&cu=INR";
    }

    uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
    uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];

    bool ok = qrcodegen_encodeText(payload, tempBuffer, qrcode,
                                   qrcodegen_Ecc_LOW, 1, 10, qrcodegen_Mask_AUTO, true);
    if (!ok) {
        ok = qrcodegen_encodeText("upi://pay?pa=voltbill.utility@axisbank&pn=VoltBill",
                                  tempBuffer, qrcode, qrcodegen_Ecc_LOW, 1, 10, qrcodegen_Mask_AUTO, true);
    }
    if (!ok) return;

    int size = qrcodegen_getSize(qrcode);
    int border = 2; /* 2 quiet zone modules on all sides */
    int total_width = size + 2 * border;

    int left_pad = (width - total_width) / 2;
    int right_pad = width - left_pad - total_width;
    if (left_pad < 0) left_pad = 0;
    if (right_pad < 0) right_pad = 0;

    /* Render using standard half-blocks on pure white background with black foreground:
       Top module: row y
       Bottom module: row y + 1
       White background (\033[47m) + Black foreground (\033[30m)
       Top dark, bottom dark  -> █
       Top dark, bottom light -> ▀
       Top light, bottom dark -> ▄
       Top light, bottom light -> ' '
    */
    for (int y = -border; y < size + border; y += 2) {
        printf("  " DBOX_V "%*s\033[47m\033[30m", left_pad, "");
        for (int x = -border; x < size + border; x++) {
            bool top = (x >= 0 && x < size && y >= 0 && y < size) ? qrcodegen_getModule(qrcode, x, y) : false;
            int y2 = y + 1;
            bool btm = (x >= 0 && x < size && y2 >= 0 && y2 < size) ? qrcodegen_getModule(qrcode, x, y2) : false;

            if (top && btm) {
                printf("█");
            } else if (top && !btm) {
                printf("▀");
            } else if (!top && btm) {
                printf("▄");
            } else {
                printf(" ");
            }
        }
        printf("\033[0m%*s" DBOX_V "\n", right_pad, "");
    }

    char upi_label[128];
    snprintf(upi_label, sizeof(upi_label), CLR_CYAN "UPI ID: voltbill.utility@axisbank" CLR_RESET " " CLR_DIM "(Scan with any UPI App)" CLR_RESET);
    int u_vis = ui_visual_len(upi_label);
    int u_lpad = (width - u_vis) / 2;
    int u_rpad = width - u_lpad - u_vis;
    if (u_lpad < 0) u_lpad = 0;
    if (u_rpad < 0) u_rpad = 0;
    printf("  " DBOX_V "%*s%s%*s" DBOX_V "\n", u_lpad, "", upi_label, u_rpad, "");

    snprintf(upi_label, sizeof(upi_label), CLR_GRAY "Bharat BillPay (BBPS) ◈ Instant Digital Settlement" CLR_RESET);
    u_vis = ui_visual_len(upi_label);
    u_lpad = (width - u_vis) / 2;
    u_rpad = width - u_lpad - u_vis;
    if (u_lpad < 0) u_lpad = 0;
    if (u_rpad < 0) u_rpad = 0;
    printf("  " DBOX_V "%*s%s%*s" DBOX_V "\n", u_lpad, "", upi_label, u_rpad, "");
}

void ui_render_fullscreen_qr(const char *title, const char *payload, const char *subtext) {
    if (!payload || strlen(payload) == 0) return;

    clear_screen();
    ui_header(title ? title : "BHARAT BILLPAY DIGITAL UPI QR", "Instant Payment & Direct Settlement");

    int card_w = 76;
    ui_card_begin(card_w, "BHARAT BILLPAY / INSTANT DIGITAL UPI");
    if (subtext && strlen(subtext) > 0) {
        ui_card_text(card_w, CLR_WHITE CLR_BOLD "%s" CLR_RESET, subtext);
        ui_card_divider(card_w);
    }

    ui_card_qr(card_w, payload);

    ui_card_divider(card_w);
    ui_card_text(card_w, CLR_YELLOW "• Supported Apps : Google Pay, PhonePe, Paytm, BHIM, Cred, Any UPI" CLR_RESET);
    ui_card_text(card_w, CLR_GREEN  "• High-Res Image : Exported to data/bills/ (BMP for printing)" CLR_RESET);
    ui_card_end(card_w);

    printf("\n  " CLR_CYAN "[Press Enter or any key to return...]" CLR_RESET " ");
    pause_prompt();
}

void ui_box_top(int width, const char *title) {
    ui_card_begin(width, title);
}

void ui_box_row(int width, const char *label, const char *value) {
    ui_card_row(width, label, value);
}

void ui_box_divider(int width) {
    ui_card_divider(width);
}

void ui_box_bottom(int width) {
    ui_card_end(width);
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
