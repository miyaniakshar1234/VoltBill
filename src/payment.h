/**
 * @file payment.h
 * @brief Payment processing, receipt generator, and transaction ledger.
 * @author Akshar Miyani
 */

#ifndef PAYMENT_H
#define PAYMENT_H

#include "models.h"

/**
 * @brief Initializes payment subsystem and records.
 */
void payment_init(void);

/**
 * @brief Returns total number of payment transactions recorded.
 */
int payment_get_count(void);

/**
 * @brief Retrieves payment by array index.
 */
PaymentRecord *payment_get_by_index(int index);

/**
 * @brief Interactive workflow to collect and settle electricity bill payments.
 */
void payment_process_flow(void);

/**
 * @brief Displays a list of all recorded payment receipts.
 */
void payment_list_all(void);

/**
 * @brief Direct CLI one-liner payment processor.
 */
int payment_quick_pay(const char *bill_or_consumer, double amount, int mode);

/**
 * @brief Renders a terminal receipt card for a transaction.
 */
void payment_render_receipt(const PaymentRecord *p, const Consumer *c, double remaining_balance);

/**
 * @brief Directly adds a payment record to memory.
 */
int payment_add_record(const PaymentRecord *p);

/**
 * @brief Clears all payment records from memory.
 */
void payment_clear_all(void);

#endif /* PAYMENT_H */
