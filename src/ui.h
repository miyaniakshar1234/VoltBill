/**
 * @file ui.h
 * @brief High-impact Cyberpunk / Volt aesthetic terminal UI library.
 * @author Akshar Miyani
 */

#ifndef UI_H
#define UI_H

/* ANSI 24-bit TrueColor / 256-Color codes */
#define CLR_RESET       "\033[0m"
#define CLR_BOLD        "\033[1m"
#define CLR_DIM         "\033[2m"
#define CLR_ITALIC      "\033[3m"
#define CLR_UNDERLINE   "\033[4m"

/* Cyberpunk Neon Palette */
#define CLR_CYAN        "\033[38;2;0;240;255m"      /* Electric Neon Cyan */
#define CLR_VIOLET      "\033[38;2;189;0;255m"     /* Cyber Violet */
#define CLR_YELLOW      "\033[38;2;255;230;0m"     /* Volt Yellow */
#define CLR_GREEN       "\033[38;2;0;255;163m"     /* Emerald Matrix Green */
#define CLR_RED         "\033[38;2;255;75;92m"      /* Coral Red Warning */
#define CLR_WHITE       "\033[38;2;240;245;255m"   /* Crisp Bright White */
#define CLR_GRAY        "\033[38;2;110;120;140m"   /* Dim Slate Gray */
#define CLR_DARK_GRAY   "\033[38;2;60;70;90m"      /* Dark Charcoal */

/* Backgrounds */
#define BG_CYAN         "\033[48;2;0;100;140m"
#define BG_VIOLET       "\033[48;2;80;0;120m"
#define BG_DARK         "\033[48;2;20;25;35m"
#define BG_WHITE_TEXT   "\033[48;2;0;240;255m\033[38;2;10;15;25m"

/* Standard Box Characters */
#define BOX_H           "─"
#define BOX_V           "│"
#define BOX_TL          "┌"
#define BOX_TR          "┐"
#define BOX_BL          "└"
#define BOX_BR          "┘"
#define BOX_T_DOWN      "┬"
#define BOX_T_UP        "┴"
#define BOX_T_RIGHT     "├"
#define BOX_T_LEFT      "┤"
#define BOX_CROSS       "┼"

/* Double Line Box Characters */
#define DBOX_H          "═"
#define DBOX_V          "║"
#define DBOX_TL         "╔"
#define DBOX_TR         "╗"
#define DBOX_BL         "╚"
#define DBOX_BR         "╝"
#define DBOX_T_RIGHT    "╠"
#define DBOX_T_LEFT     "╣"
#define DBOX_CROSS      "╬"

/**
 * @brief Draws a page header with title and breadcrumb subtitle.
 */
void ui_header(const char *title, const char *subtitle);

/**
 * @brief Draws a small top title bar.
 */
void ui_box_top(int width, const char *title);

/**
 * @brief Draws an itemized row within a box with label on left, value on right.
 */
void ui_box_row(int width, const char *label, const char *value);

/**
 * @brief Draws a horizontal divider line inside a box.
 */
void ui_box_divider(int width);

/**
 * @brief Draws the bottom closing border of a box.
 */
void ui_box_bottom(int width);

/**
 * @brief Draws a bottom status bar with system metadata and author credit.
 */
void ui_status_bar(const char *left_text, const char *right_text);

/**
 * @brief Renders a colored status badge, e.g. [ACTIVE], [PAID].
 */
void ui_badge(const char *label, const char *color);

/**
 * @brief Renders a sleek terminal progress bar.
 */
void ui_progress_bar(double percent, int bar_width);

/**
 * @brief Interactive arrow-key menu with real-time selection highlight.
 * @param title Menu header title
 * @param options Array of string option labels
 * @param count Number of options
 * @param default_idx Initially highlighted index
 * @return 0-based selected index
 */
int ui_menu(const char *title, const char **options, int count, int default_idx);

/**
 * @brief Displays an alert message box (Success, Info, or Error).
 */
void ui_message_box(const char *title, const char *message, int is_success);

#endif /* UI_H */
