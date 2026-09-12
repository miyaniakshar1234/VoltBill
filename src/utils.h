/**
 * @file utils.h
 * @brief Cross-platform utilities, input handling, and terminal abstractions.
 * @author Akshar Miyani
 */

#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

/* Key definitions for interactive menu */
#define KEY_UP       1001
#define KEY_DOWN     1002
#define KEY_LEFT     1003
#define KEY_RIGHT    1004
#define KEY_ENTER    10
#define KEY_ESC      27
#define KEY_BACKSPACE 8

/**
 * @brief Initializes console for ANSI colors, UTF-8 encoding, and raw input.
 */
void init_console(void);

/**
 * @brief Restores console state prior to application exit.
 */
void restore_console(void);

/**
 * @brief Clears the terminal screen smoothly.
 */
void clear_screen(void);

/**
 * @brief Halts execution until user presses any key.
 */
void pause_prompt(void);

/**
 * @brief Non-blocking or blocking single keystroke reader supporting arrow keys.
 * @return Key code (KEY_UP, KEY_DOWN, KEY_ENTER, KEY_ESC, or ASCII value).
 */
int read_key(void);

/**
 * @brief Safely reads a line of string from stdin without buffer overflow.
 * @param dest Output buffer
 * @param max_len Size of dest buffer
 */
void get_safe_string(char *dest, size_t max_len);

/**
 * @brief Prompts user for a floating point number within [min_val, max_val].
 */
double get_safe_double(const char *prompt, double min_val, double max_val);

/**
 * @brief Prompts user for an integer within [min_val, max_val].
 */
int get_safe_int(const char *prompt, int min_val, int max_val);

/**
 * @brief Gets current timestamp formatted as "YYYY-MM-DD HH:MM:SS".
 */
void get_current_timestamp(char *buffer, size_t len);

/**
 * @brief Gets current date formatted as "YYYY-MM-DD".
 */
void get_current_date(char *buffer, size_t len);

/**
 * @brief Computes a due date offset by days_ahead from base_date.
 */
void compute_due_date(const char *base_date, int days_ahead, char *buffer, size_t len);

/**
 * @brief Gets current billing cycle string e.g. "SEP-2026".
 */
void get_current_billing_cycle(char *buffer, size_t len);

/**
 * @brief Formats currency amount as "Rs. 1,234.50".
 */
void format_currency(double amount, char *buffer, size_t len);

/**
 * @brief Ensures a directory exists on disk.
 */
int ensure_directory(const char *path);

/**
 * @brief Sleeps for specified milliseconds.
 */
void sleep_ms(int milliseconds);

/**
 * @brief Appends an entry to the system audit trail log (data/audit_trail.log).
 */
void audit_log(const char *action, const char *details);

/**
 * @brief Returns 1 if given due_date (YYYY-MM-DD) is earlier than current system date.
 */
int is_date_overdue(const char *due_date);

#endif /* UTILS_H */
