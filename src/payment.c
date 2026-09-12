/**
 * @file payment.c
 * @brief Implementation of payment transaction processing and receipts.
 * @author Akshar Miyani
 */

#include "payment.h"
#include "billing.h"
#include "customer.h"
#include "ui.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static PaymentRecord g_payments[MAX_PAYMENTS];
static int g_payment_count = 0;

void payment_init(void) {
    g_payment_count = 0;
}

int payment_get_count(void) {
    return g_payment_count;
}

PaymentRecord *payment_get_by_index(int index) {
    if (index >= 0 && index < g_payment_count) {
        return &g_payments[index];
    }
    return NULL;
}

int payment_add_record(const PaymentRecord *p) {
    if (g_payment_count >= MAX_PAYMENTS || !p) return 0;
    g_payments[g_payment_count++] = *p;
    return 1;
}

void payment_clear_all(void) {
    g_payment_count = 0;
}

static const char *payment_mode_str(PaymentMode mode) {
    switch (mode) {
        case PAY_CASH:              return "Cash Over Counter";
        case PAY_UPI:               return "UPI (Unified Payments)";
        case PAY_CREDIT_DEBIT_CARD: return "Credit / Debit Card";
        case PAY_NET_BANKING:       return "Internet Banking";
        default:                    return "Electronic Transfer";
    }
}

void payment_render_receipt(const PaymentRecord *p, const Consumer *c, double remaining_balance) {
    if (!p || !c) return;

    printf("\n");
    printf("  " DBOX_TL);
    for (int i = 0; i < 70; i++) printf(DBOX_H);
    printf(DBOX_TR "\n");

    printf("  " DBOX_V "  " CLR_GREEN CLR_BOLD "VOLTBILL OFFICIAL PAYMENT RECEIPT" CLR_RESET "                                " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_GRAY "Payment Settlement Verification Ledger • VoltBill Utility Engine" CLR_RESET "     " DBOX_V "\n");

    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 70; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    printf("  " DBOX_V "  " CLR_WHITE "Receipt Number  :" CLR_RESET " " CLR_CYAN CLR_BOLD "%-48s" CLR_RESET " " DBOX_V "\n", p->receipt_id);
    printf("  " DBOX_V "  " CLR_WHITE "Associated Bill :" CLR_RESET " %-48s " DBOX_V "\n", p->bill_id);
    printf("  " DBOX_V "  " CLR_WHITE "Consumer ID     :" CLR_RESET " %-48s " DBOX_V "\n", c->id);
    printf("  " DBOX_V "  " CLR_WHITE "Consumer Name   :" CLR_RESET " %-48.48s " DBOX_V "\n", c->name);
    printf("  " DBOX_V "  " CLR_WHITE "Payment Mode    :" CLR_RESET " %-48s " DBOX_V "\n", payment_mode_str(p->mode));
    printf("  " DBOX_V "  " CLR_WHITE "Transaction Ref :" CLR_RESET " %-48s " DBOX_V "\n", p->transaction_ref);
    printf("  " DBOX_V "  " CLR_WHITE "Date & Time     :" CLR_RESET " %-48s " DBOX_V "\n", p->payment_date);

    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 70; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    char amt_str[32], bal_str[32];
    format_currency(p->amount, amt_str, sizeof(amt_str));
    format_currency(remaining_balance, bal_str, sizeof(bal_str));

    printf("  " DBOX_V "  " CLR_WHITE CLR_BOLD "AMOUNT RECEIVED :" CLR_RESET " " 
           CLR_GREEN CLR_BOLD "%-48s" CLR_RESET " " DBOX_V "\n", amt_str);
    printf("  " DBOX_V "  " CLR_GRAY "Outstanding Balance Remaining : %-37s" CLR_RESET " " DBOX_V "\n", bal_str);

    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 70; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    printf("  " DBOX_V "  " CLR_GREEN "STATUS: TRANSACTION VERIFIED & SETTLED IN UTILITY LEDGER" CLR_RESET "           " DBOX_V "\n");

    printf("  " DBOX_BL);
    for (int i = 0; i < 70; i++) printf(DBOX_H);
    printf(DBOX_BR "\n");

    printf("  " CLR_GRAY "Engineered by Akshar Miyani" CLR_RESET "\n\n");
}

int payment_quick_pay(const char *bill_or_consumer, double amount, int mode) {
    BillBreakdown *bill = billing_find_by_id(bill_or_consumer);
    Consumer *c = NULL;

    if (bill) {
        c = customer_find_by_id(bill->consumer_id);
    } else {
        c = customer_find_by_id(bill_or_consumer);
        if (c) bill = billing_get_latest_for_consumer(c->id);
    }

    if (!c) {
        printf("VoltBill: Target '%s' not found.\n", bill_or_consumer);
        return 0;
    }

    PaymentRecord p;
    memset(&p, 0, sizeof(p));
    snprintf(p.receipt_id, sizeof(p.receipt_id), "REC-%04d", g_payment_count + 1001);
    strncpy(p.bill_id, bill ? bill->bill_id : "DIRECT", BILL_ID_LEN - 1);
    strncpy(p.consumer_id, c->id, ID_LEN - 1);
    get_current_timestamp(p.payment_date, sizeof(p.payment_date));
    p.amount = amount;
    p.mode = (PaymentMode)mode;
    snprintf(p.transaction_ref, sizeof(p.transaction_ref), "CLI-PAY-%06d", rand() % 1000000);

    if (bill) {
        bill->amount_paid += amount;
        if (bill->amount_paid >= bill->net_payable_amount) bill->status = BILL_PAID;
        else bill->status = BILL_PARTIALLY_PAID;
    }

    if (amount >= c->outstanding_arrears) {
        c->advance_credit += (amount - c->outstanding_arrears);
        c->outstanding_arrears = 0.0;
        c->is_flagged_for_disconnection = 0;
    } else {
        c->outstanding_arrears -= amount;
    }

    payment_add_record(&p);

    extern int storage_save_all(void);
    storage_save_all();

    char audit_desc[128];
    snprintf(audit_desc, sizeof(audit_desc), "CLI Payment %s for %s (Amount: Rs. %.2f)", p.receipt_id, c->id, amount);
    audit_log("CLI_PAYMENT", audit_desc);

    printf("VoltBill: Payment recorded! Receipt: %s | Amount: Rs. %.2f | Balance: Rs. %.2f\n",
           p.receipt_id, amount, c->outstanding_arrears);
    return 1;
}

void payment_process_flow(void) {
    ui_header("BILL PAYMENT & LEDGER SETTLEMENT", "Process Consumer Dues via Cash, UPI, Card, or NetBanking");

    printf("  " CLR_WHITE "Enter Bill ID (e.g. BILL-1001) or Consumer ID (e.g. VB-1001): " CLR_RESET);
    char query[64];
    get_safe_string(query, sizeof(query));

    if (query[0] == '\0') return;

    BillBreakdown *bill = billing_find_by_id(query);
    Consumer *consumer = NULL;

    if (bill) {
        consumer = customer_find_by_id(bill->consumer_id);
    } else {
        consumer = customer_find_by_id(query);
        if (consumer) {
            bill = billing_get_latest_for_consumer(consumer->id);
        }
    }

    if (!consumer) {
        ui_message_box("Not Found", "No matching consumer or bill found.", 0);
        return;
    }

    customer_render_card(consumer);

    double amount_due = 0.0;
    if (bill && bill->status != BILL_PAID) {
        amount_due = bill->net_payable_amount - bill->amount_paid;
    } else {
        amount_due = consumer->outstanding_arrears;
    }

    if (amount_due <= 0.0) {
        ui_message_box("No Dues Pending", "This consumer has zero outstanding dues.", 1);
        return;
    }

    printf("  " CLR_WHITE "Outstanding Amount Due: " CLR_YELLOW CLR_BOLD "₹ %.2f" CLR_RESET "\n\n", amount_due);

    double pay_amount = get_safe_double("  Enter Payment Amount to Collect (₹): ₹ ", 1.0, 1000000.0);

    printf("\n  " CLR_CYAN "Payment Channel / Mode:" CLR_RESET "\n");
    printf("    [0] Cash Payment\n");
    printf("    [1] UPI (Google Pay / PhonePe / Paytm / BHIM)\n");
    printf("    [2] Credit / Debit Card (POS)\n");
    printf("    [3] Internet Net Banking\n");
    int mode_choice = get_safe_int("  Select Channel [0-3]: ", 0, 3);

    char tx_ref[32];
    if (mode_choice == 1) {
        printf("  Enter UPI Transaction ID (or press Enter for auto): ");
        get_safe_string(tx_ref, sizeof(tx_ref));
        if (tx_ref[0] == '\0') {
            snprintf(tx_ref, sizeof(tx_ref), "UPI-%ld", (long)(rand() % 90000000 + 10000000));
        }
    } else if (mode_choice == 2) {
        printf("  Enter Card Auth Code / Last 4 Digits: ");
        get_safe_string(tx_ref, sizeof(tx_ref));
        if (tx_ref[0] == '\0') snprintf(tx_ref, sizeof(tx_ref), "CARD-POS-%04d", rand() % 10000);
    } else if (mode_choice == 3) {
        printf("  Enter Bank Reference Number: ");
        get_safe_string(tx_ref, sizeof(tx_ref));
        if (tx_ref[0] == '\0') snprintf(tx_ref, sizeof(tx_ref), "NETB-%08d", rand() % 10000000);
    } else {
        snprintf(tx_ref, sizeof(tx_ref), "CASH-COUNTER-%04d", g_payment_count + 1);
    }

    PaymentRecord p;
    memset(&p, 0, sizeof(p));
    snprintf(p.receipt_id, sizeof(p.receipt_id), "REC-%04d", g_payment_count + 1001);
    strncpy(p.bill_id, bill ? bill->bill_id : "DIRECT", BILL_ID_LEN - 1);
    strncpy(p.consumer_id, consumer->id, ID_LEN - 1);
    get_current_timestamp(p.payment_date, sizeof(p.payment_date));
    p.amount = pay_amount;
    p.mode = (PaymentMode)mode_choice;
    strncpy(p.transaction_ref, tx_ref, sizeof(p.transaction_ref) - 1);

    if (bill) {
        bill->amount_paid += pay_amount;
        if (bill->amount_paid >= bill->net_payable_amount) {
            bill->status = BILL_PAID;
        } else {
            bill->status = BILL_PARTIALLY_PAID;
        }
    }

    if (pay_amount >= consumer->outstanding_arrears) {
        double excess = pay_amount - consumer->outstanding_arrears;
        consumer->outstanding_arrears = 0.0;
        consumer->advance_credit += excess;
        consumer->is_flagged_for_disconnection = 0;
    } else {
        consumer->outstanding_arrears -= pay_amount;
    }

    payment_add_record(&p);

    extern int storage_save_all(void);
    storage_save_all();

    char audit_desc[128];
    snprintf(audit_desc, sizeof(audit_desc), "Processed %s for %s (Amount: Rs. %.2f, Mode: %d)",
             p.receipt_id, consumer->id, pay_amount, mode_choice);
    audit_log("PAYMENT_COLLECTED", audit_desc);

    clear_screen();
    payment_render_receipt(&p, consumer, consumer->outstanding_arrears);
    pause_prompt();
}

void payment_list_all(void) {
    ui_header("PAYMENT TRANSACTIONS & RECEIPTS", "Audit Log of All Settled Energy Payments");

    if (g_payment_count == 0) {
        printf("  " CLR_YELLOW "No payment transactions recorded yet." CLR_RESET "\n");
        pause_prompt();
        return;
    }

    printf("  " CLR_GRAY "┌──────────────┬──────────────┬────────────┬──────────────────────┬─────────────┬─────────────┐" CLR_RESET "\n");
    printf("  " CLR_GRAY "│ " CLR_CYAN CLR_BOLD "%-12s" CLR_RESET CLR_GRAY "│ " 
           CLR_WHITE CLR_BOLD "%-12s" CLR_RESET CLR_GRAY "│ " 
           CLR_YELLOW CLR_BOLD "%-10s" CLR_RESET CLR_GRAY "│ " 
           CLR_WHITE CLR_BOLD "%-20s" CLR_RESET CLR_GRAY "│ " 
           CLR_GREEN CLR_BOLD "%-11s" CLR_RESET CLR_GRAY "│ " 
           CLR_VIOLET CLR_BOLD "%-11s" CLR_RESET CLR_GRAY "│" CLR_RESET "\n",
           "Receipt ID", "Bill ID", "Consumer", "Timestamp", "Amount (₹)", "Mode");
    printf("  " CLR_GRAY "├──────────────┼──────────────┼────────────┼──────────────────────┼─────────────┼─────────────┤" CLR_RESET "\n");

    for (int i = 0; i < g_payment_count; i++) {
        PaymentRecord *p = &g_payments[i];
        const char *m = "Cash";
        if (p->mode == PAY_UPI) m = "UPI";
        else if (p->mode == PAY_CREDIT_DEBIT_CARD) m = "Card";
        else if (p->mode == PAY_NET_BANKING) m = "NetBank";

        printf("  " CLR_GRAY "│ " CLR_CYAN "%-12s" CLR_RESET CLR_GRAY "│ " 
               CLR_WHITE "%-12s" CLR_RESET CLR_GRAY "│ " 
               CLR_YELLOW "%-10s" CLR_RESET CLR_GRAY "│ " 
               CLR_GRAY "%-20s" CLR_RESET CLR_GRAY "│ " 
               CLR_GREEN "%11.2f" CLR_RESET CLR_GRAY "│ " 
               CLR_WHITE "%-11s" CLR_RESET CLR_GRAY "│" CLR_RESET "\n",
               p->receipt_id, p->bill_id, p->consumer_id, p->payment_date, p->amount, m);
    }

    printf("  " CLR_GRAY "└──────────────┴──────────────┴────────────┴──────────────────────┴─────────────┴─────────────┘" CLR_RESET "\n");
    printf("  " CLR_GRAY "Total Receipts Issued: " CLR_CYAN CLR_BOLD "%d" CLR_RESET "\n", g_payment_count);

    pause_prompt();
}
