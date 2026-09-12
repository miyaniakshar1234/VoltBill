/**
 * @file storage.h
 * @brief Binary persistence, CSV export ledger, and demo data seeder.
 * @author Akshar Miyani
 */

#ifndef STORAGE_H
#define STORAGE_H

#include <stddef.h>

/**
 * @brief Initializes storage engine and loads persisted data from data/ directory.
 */
void storage_init(void);

/**
 * @brief Saves all consumers, bills, and payment records to disk.
 * @return 1 on success, 0 on failure.
 */
int storage_save_all(void);

/**
 * @brief Loads all data records from disk into active memory.
 * @return 1 on success, 0 on failure.
 */
int storage_load_all(void);

/**
 * @brief Exports all consumers and bills to CSV spreadsheets in data/ directory.
 */
int storage_export_csv(void);

/**
 * @brief Creates a full timestamped backup archive of the entire database.
 */
int storage_create_backup(char *out_path, size_t max_len);

/**
 * @brief Exports all consumers and bills into a clean JSON document.
 */
int storage_export_json(const char *filepath);

/**
 * @brief Seeds rich realistic consumers, historical readings, and bills for demo.
 */
void storage_seed_demo_data(void);

/**
 * @brief Displays storage diagnostics, record counts, and database size on disk.
 */
void storage_display_stats(void);

#endif /* STORAGE_H */
