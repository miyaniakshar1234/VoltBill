/**
 * @file billing.h
 * @brief Meter reading ingestion, progressive slab calculation, and invoice generator.
 * @author Akshar Miyani
 */

#ifndef BILLING_H
#define BILLING_H

#include "models.h"

/**
 * @brief Initializes billing and meter reading records.
 */
void billing_init(void);

/**
 * @brief Returns total number of generated bills.
 */
int billing_get_count(void);

/**
 * @brief Retrieves bill by array index.
 */
BillBreakdown *billing_get_by_index(int index);

/**
 * @brief Finds bill by unique Bill ID.
 */
BillBreakdown *billing_find_by_id(const char *bill_id);

/**
 * @brief Finds latest bill for a consumer.
 */
BillBreakdown *billing_get_latest_for_consumer(const char *consumer_id);

/**
 * @brief Interactive workflow to enter meter reading and generate an itemized bill.
 */
void billing_generate_flow(void);

/**
 * @brief Batch generates bills for all active registered consumers across the grid.
 */
void billing_batch_generate_flow(void);

/**
 * @brief Interactive search and filter engine for past bills.
 */
void billing_filter_flow(void);

/**
 * @brief Direct CLI one-liner bill generator.
 */
int billing_quick_bill(const char *consumer_id, double curr_reading);

/**
 * @brief Computes comprehensive bill breakdown for given units and consumer parameters.
 */
void billing_calculate(const Consumer *c, double prev_reading, double curr_reading,
                       double solar_units, double peak_units, double power_factor, BillBreakdown *out_bill);

/**
 * @brief Interactive Tariff & Bill Simulator (What-if calculator without registering a consumer).
 */
void billing_simulator_flow(void);

/**
 * @brief Instant CLI tariff calculation (e.g. voltbill calc 250).
 */
void billing_quick_calc(double units, int category_idx, double solar_units);

/**
 * @brief Displays a lavish terminal invoice for a bill.
 */
void billing_render_invoice(const BillBreakdown *b, const Consumer *c);

/**
 * @brief Exports an invoice to a standalone text file in data/bills/.
 */
int billing_export_text_invoice(const BillBreakdown *b, const Consumer *c);

/**
 * @brief Displays history of all generated bills with status badges.
 */
void billing_list_all(void);

/**
 * @brief Directly adds a bill record to memory (used by storage & seeder).
 */
int billing_add_record(const BillBreakdown *b);

/**
 * @brief Clears all bill records from memory.
 */
void billing_clear_all(void);

#endif /* BILLING_H */
