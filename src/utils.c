/**
 * @file utils.c
 * @brief Implementation of cross-platform utility routines.
 * @author Akshar Miyani
 */

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#ifdef _WIN32
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  #include <windows.h>
  #include <conio.h>
  #include <direct.h>
  #include <io.h>
  static DWORD original_out_mode = 0;
  static UINT original_cp = 0;
  static UINT original_output_cp = 0;
#else
  #include <termios.h>
  #include <unistd.h>
  #include <sys/stat.h>
  #include <sys/types.h>
  static struct termios orig_termios;
  static int raw_mode_active = 0;
#endif

void init_console(void) {
#ifdef _WIN32
    original_cp = GetConsoleCP();
    original_output_cp = GetConsoleOutputCP();
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD mode = 0;
        if (GetConsoleMode(hOut, &mode)) {
            original_out_mode = mode;
            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, mode);
        }
    }
#else
    /* Unix terminal UTF-8 is default */
#endif
    /* Ensure data and config directories exist */
    ensure_directory("data");
    ensure_directory("config");
    ensure_directory("data/bills");
    ensure_directory("data/notices");
    ensure_directory("data/backups");
}

void restore_console(void) {
#ifdef _WIN32
    if (original_out_mode != 0) {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            SetConsoleMode(hOut, original_out_mode);
        }
    }
    if (original_cp != 0) SetConsoleCP(original_cp);
    if (original_output_cp != 0) SetConsoleOutputCP(original_output_cp);
#else
    if (raw_mode_active) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
        raw_mode_active = 0;
    }
#endif
}

void clear_screen(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}

void sleep_ms(int milliseconds) {
#ifdef _WIN32
    Sleep((DWORD)milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

int read_key(void) {
#ifdef _WIN32
    int ch = _getch();
    if (ch == 0 || ch == 224) {
        ch = _getch();
        switch (ch) {
            case 72: return KEY_UP;
            case 80: return KEY_DOWN;
            case 75: return KEY_LEFT;
            case 77: return KEY_RIGHT;
            default: return ch;
        }
    }
    if (ch == 13) return KEY_ENTER;
    if (ch == 27) return KEY_ESC;
    if (ch == 8)  return KEY_BACKSPACE;
    return ch;
#else
    struct termios raw;
    tcgetattr(STDIN_FILENO, &orig_termios);
    raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    int ch = getchar();
    if (ch == 27) {
        int seq1 = getchar();
        if (seq1 == '[') {
            int seq2 = getchar();
            switch (seq2) {
                case 'A': ch = KEY_UP; break;
                case 'B': ch = KEY_DOWN; break;
                case 'C': ch = KEY_RIGHT; break;
                case 'D': ch = KEY_LEFT; break;
                default: break;
            }
        } else {
            ch = KEY_ESC;
        }
    } else if (ch == '\n' || ch == '\r') {
        ch = KEY_ENTER;
    } else if (ch == 127) {
        ch = KEY_BACKSPACE;
    }

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    return ch;
#endif
}

void pause_prompt(void) {
#ifdef _WIN32
    if (!_isatty(_fileno(stdin))) return;
#else
    if (!isatty(STDIN_FILENO)) return;
#endif
    printf("\n  \033[90m[Press any key to continue...]\033[0m ");
    fflush(stdout);
    read_key();
}

void get_safe_string(char *dest, size_t max_len) {
    if (!dest || max_len == 0) return;
    dest[0] = '\0';

    char buffer[512];
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
            buffer[len - 1] = '\0';
            len--;
        }
        if (len > 0 && buffer[len - 1] == '\r') {
            buffer[len - 1] = '\0';
            len--;
        }

        char *start = buffer;
        while (*start && isspace((unsigned char)*start)) start++;

        char *end = start + strlen(start) - 1;
        while (end > start && isspace((unsigned char)*end)) {
            *end = '\0';
            end--;
        }

        strncpy(dest, start, max_len - 1);
        dest[max_len - 1] = '\0';
    }
}

double get_safe_double(const char *prompt, double min_val, double max_val) {
    char input[64];
    double val = 0.0;
    while (1) {
        printf("%s", prompt);
        fflush(stdout);
        get_safe_string(input, sizeof(input));
        if (input[0] == '\0') {
            printf("  \033[91m[Error] Input cannot be blank. Try again.\033[0m\n");
            continue;
        }
        char *endptr;
        val = strtod(input, &endptr);
        if (*endptr != '\0') {
            printf("  \033[91m[Error] Invalid numerical input. Please enter a valid number.\033[0m\n");
            continue;
        }
        if (val < min_val || val > max_val) {
            printf("  \033[91m[Error] Value must be between %.2f and %.2f.\033[0m\n", min_val, max_val);
            continue;
        }
        return val;
    }
}

int get_safe_int(const char *prompt, int min_val, int max_val) {
    char input[64];
    int val = 0;
    while (1) {
        printf("%s", prompt);
        fflush(stdout);
        get_safe_string(input, sizeof(input));
        if (input[0] == '\0') {
            printf("  \033[91m[Error] Input cannot be blank. Try again.\033[0m\n");
            continue;
        }
        char *endptr;
        long parsed = strtol(input, &endptr, 10);
        if (*endptr != '\0') {
            printf("  \033[91m[Error] Invalid integer input. Please enter a whole number.\033[0m\n");
            continue;
        }
        val = (int)parsed;
        if (val < min_val || val > max_val) {
            printf("  \033[91m[Error] Number must be between %d and %d.\033[0m\n", min_val, max_val);
            continue;
        }
        return val;
    }
}

void get_current_timestamp(char *buffer, size_t len) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    if (tm_info) {
        strftime(buffer, len, "%Y-%m-%d %H:%M:%S", tm_info);
    } else {
        snprintf(buffer, len, "2026-09-12 12:00:00");
    }
}

void get_current_date(char *buffer, size_t len) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    if (tm_info) {
        strftime(buffer, len, "%Y-%m-%d", tm_info);
    } else {
        snprintf(buffer, len, "2026-09-12");
    }
}

void compute_due_date(const char *base_date, int days_ahead, char *buffer, size_t len) {
    int y, m, d;
    if (sscanf(base_date, "%d-%d-%d", &y, &m, &d) == 3) {
        struct tm tm_date = {0};
        tm_date.tm_year = y - 1900;
        tm_date.tm_mon = m - 1;
        tm_date.tm_mday = d + days_ahead;
        time_t t = mktime(&tm_date);
        struct tm *res = localtime(&t);
        if (res) {
            strftime(buffer, len, "%Y-%m-%d", res);
            return;
        }
    }
    snprintf(buffer, len, "%s", base_date);
}

int is_date_overdue(const char *due_date) {
    char current_date[32];
    get_current_date(current_date, sizeof(current_date));
    return strcmp(due_date, current_date) < 0;
}

void get_current_billing_cycle(char *buffer, size_t len) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    if (tm_info) {
        strftime(buffer, len, "%b-%Y", tm_info);
        for (char *p = buffer; *p; ++p) *p = (char)toupper((unsigned char)*p);
    } else {
        snprintf(buffer, len, "SEP-2026");
    }
}

void format_currency(double amount, char *buffer, size_t len) {
    snprintf(buffer, len, "Rs. %.2f", amount);
}

int ensure_directory(const char *path) {
#ifdef _WIN32
    return _mkdir(path);
#else
    return mkdir(path, 0755);
#endif
}

void audit_log(const char *action, const char *details) {
    ensure_directory("data");
    FILE *fp = fopen("data/audit_trail.log", "a");
    if (!fp) return;

    char timestamp[32];
    get_current_timestamp(timestamp, sizeof(timestamp));
    fprintf(fp, "[%s] ACTION: %-20s | DETAILS: %s\n", timestamp, action, details ? details : "");
    fclose(fp);
}
