/**
 * @file storage.h
 * @brief Binary persistence, CSV export ledger, and academic demo data seeder.
 * @author Akshar Miyani (MCA 1st Sem, Manipal University Jaipur)
 */

#ifndef STORAGE_H
#define STORAGE_H

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
 * @brief Seeds rich realistic consumers, historical readings, and bills for professor demo.
 */
void storage_seed_demo_data(void);

/**
 * @brief Displays storage diagnostics, record counts, and database size on disk.
 */
void storage_display_stats(void);

#endif /* STORAGE_H */
