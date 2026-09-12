/**
 * @file customer.h
 * @brief Customer registration, profile lookup, search, and lifecycle management.
 * @author Akshar Miyani
 */

#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "models.h"

/**
 * @brief Initializes customer module and memory records.
 */
void customer_init(void);

/**
 * @brief Returns total number of registered customers.
 */
int customer_get_count(void);

/**
 * @brief Retrieves customer by array index (0 <= index < count).
 */
Consumer *customer_get_by_index(int index);

/**
 * @brief Finds customer by Consumer ID (e.g., "VB-1001").
 */
Consumer *customer_find_by_id(const char *id);

/**
 * @brief Finds customer by Meter Serial Number.
 */
Consumer *customer_find_by_meter(const char *meter_no);

/**
 * @brief Interactive workflow to register a new customer.
 */
void customer_register_flow(void);

/**
 * @brief Interactive workflow to search customer by ID, Name, Phone, or Meter.
 */
void customer_search_flow(void);

/**
 * @brief Displays a tabular list of all registered consumers.
 */
void customer_list_all(void);

/**
 * @brief Interactive workflow to edit an existing customer profile.
 */
void customer_edit_flow(void);

/**
 * @brief Workflow to inspect high arrears and issue disconnection notices.
 */
void customer_defaulters_flow(void);

/**
 * @brief Displays quick single-screen status for CLI queries.
 */
void customer_quick_status(const char *consumer_id);

/**
 * @brief Renders a detailed graphical profile card for a customer.
 */
void customer_render_card(const Consumer *c);

/**
 * @brief Directly adds a consumer record to memory (used by storage & seeder).
 */
int customer_add_record(const Consumer *c);

/**
 * @brief Clears all customer records from memory.
 */
void customer_clear_all(void);

#endif /* CUSTOMER_H */
