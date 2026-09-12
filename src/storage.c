/**
 * @file storage.c
 * @brief Implementation of binary data storage, CSV reporting, and professor demo seeder.
 * @author Akshar Miyani (MCA 1st Sem, Manipal University Jaipur)
 */

#include "storage.h"
#include "customer.h"
#include "billing.h"
#include "payment.h"
#include "tariff.h"
#include "ui.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

static const char *CONSUMER_DAT = "data/consumers.dat";
static const char *BILLS_DAT    = "data/bills.dat";
static const char *PAYMENTS_DAT = "data/payments.dat";

void storage_init(void) {
    ensure_directory("data");
    ensure_directory("data/bills");
    storage_load_all();
}

int storage_save_all(void) {
    ensure_directory("data");

    /* 1. Save Consumers */
    FILE *fp = fopen(CONSUMER_DAT, "wb");
    if (fp) {
        int count = customer_get_count();
        fwrite(&count, sizeof(int), 1, fp);
        for (int i = 0; i < count; i++) {
            Consumer *c = customer_get_by_index(i);
            if (c) fwrite(c, sizeof(Consumer), 1, fp);
        }
        fclose(fp);
    }

    /* 2. Save Bills */
    fp = fopen(BILLS_DAT, "wb");
    if (fp) {
        int count = billing_get_count();
        fwrite(&count, sizeof(int), 1, fp);
        for (int i = 0; i < count; i++) {
            BillBreakdown *b = billing_get_by_index(i);
            if (b) fwrite(b, sizeof(BillBreakdown), 1, fp);
        }
        fclose(fp);
    }

    /* 3. Save Payments */
    fp = fopen(PAYMENTS_DAT, "wb");
    if (fp) {
        int count = payment_get_count();
        fwrite(&count, sizeof(int), 1, fp);
        for (int i = 0; i < count; i++) {
            PaymentRecord *p = payment_get_by_index(i);
            if (p) fwrite(p, sizeof(PaymentRecord), 1, fp);
        }
        fclose(fp);
    }

    return 1;
}

int storage_load_all(void) {
    /* 1. Load Consumers */
    FILE *fp = fopen(CONSUMER_DAT, "rb");
    if (fp) {
        customer_clear_all();
        int count = 0;
        if (fread(&count, sizeof(int), 1, fp) == 1) {
            for (int i = 0; i < count; i++) {
                Consumer c;
                if (fread(&c, sizeof(Consumer), 1, fp) == 1) {
                    customer_add_record(&c);
                }
            }
        }
        fclose(fp);
    }

    /* 2. Load Bills */
    fp = fopen(BILLS_DAT, "rb");
    if (fp) {
        billing_clear_all();
        int count = 0;
        if (fread(&count, sizeof(int), 1, fp) == 1) {
            for (int i = 0; i < count; i++) {
                BillBreakdown b;
                if (fread(&b, sizeof(BillBreakdown), 1, fp) == 1) {
                    billing_add_record(&b);
                }
            }
        }
        fclose(fp);
    }

    /* 3. Load Payments */
    fp = fopen(PAYMENTS_DAT, "rb");
    if (fp) {
        payment_clear_all();
        int count = 0;
        if (fread(&count, sizeof(int), 1, fp) == 1) {
            for (int i = 0; i < count; i++) {
                PaymentRecord p;
                if (fread(&p, sizeof(PaymentRecord), 1, fp) == 1) {
                    payment_add_record(&p);
                }
            }
        }
        fclose(fp);
    }

    return 1;
}

int storage_export_csv(void) {
    ensure_directory("data");

    /* Export Consumers CSV */
    FILE *fp_c = fopen("data/export_consumers.csv", "w");
    if (fp_c) {
        fprintf(fp_c, "ConsumerID,Name,Phone,Email,Address,MeterNo,Category,Phase,SanctionedLoadKW,SolarKW,OutstandingArrears,AdvanceCredit,RegisteredDate,Status\n");
        int count = customer_get_count();
        for (int i = 0; i < count; i++) {
            Consumer *c = customer_get_by_index(i);
            fprintf(fp_c, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%d,%.2f,%.2f,%.2f,%.2f,\"%s\",\"%s\"\n",
                    c->id, c->name, c->phone, c->email, c->address, c->meter_no,
                    category_to_string(c->category), c->phase, c->sanctioned_load_kw,
                    c->solar_capacity_kw, c->outstanding_arrears, c->advance_credit,
                    c->registered_date, c->is_active ? "Active" : "Inactive");
        }
        fclose(fp_c);
    }

    /* Export Bills CSV */
    FILE *fp_b = fopen("data/export_bills.csv", "w");
    if (fp_b) {
        fprintf(fp_b, "BillID,ConsumerID,BillingCycle,BillDate,DueDate,PrevReading,CurrReading,GrossUnits,SolarUnits,BilledUnits,EnergyCharges,FixedCharges,MeterRent,RegulatorySurcharge,Duty,GreenCess,FPPCA,NetPayable,AmountPaid,Status\n");
        int count = billing_get_count();
        for (int i = 0; i < count; i++) {
            BillBreakdown *b = billing_get_by_index(i);
            const char *st = "Pending";
            if (b->status == BILL_PAID) st = "Paid";
            else if (b->status == BILL_PARTIALLY_PAID) st = "Partial";
            else if (b->status == BILL_OVERDUE) st = "Overdue";

            fprintf(fp_b, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%.1f,%.1f,%.1f,%.1f,%.1f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,\"%s\"\n",
                    b->bill_id, b->consumer_id, b->billing_cycle, b->bill_date, b->due_date,
                    b->prev_reading, b->curr_reading, b->gross_units, b->solar_units, b->billed_units,
                    b->total_energy_charges, b->fixed_charges, b->meter_rent, b->regulatory_surcharge,
                    b->electricity_duty, b->green_cess, b->fppca_charges, b->net_payable_amount,
                    b->amount_paid, st);
        }
        fclose(fp_b);
    }

    return 1;
}

void storage_seed_demo_data(void) {
    ui_header("PROFESSOR EVALUATION DEMO DATA SEEDER", "Populate Grid with Realistic Multi-Category Consumers");

    printf("  " CLR_WHITE "This will seed 6 diverse consumers across all categories (Residential," CLR_RESET "\n");
    printf("  " CLR_WHITE "Commercial, Industrial, Agricultural, and Rooftop Solar Net Metering)" CLR_RESET "\n");
    printf("  " CLR_WHITE "along with 3 historical billing cycles and payment ledgers." CLR_RESET "\n\n");

    int confirm = get_safe_int("  Proceed with seeding demo records? [1 = Yes, 0 = No]: ", 0, 1);
    if (!confirm) return;

    customer_clear_all();
    billing_clear_all();
    payment_clear_all();

    /* 1. Domestic with Solar */
    Consumer c1 = {
        "VB-1001", "Dr. Rajesh Sharma", "+91 9829012345", "rajesh.sharma@example.com",
        "Flat 402, Royal Palms, C-Scheme, Jaipur", "MTR-DOM-8812",
        CAT_DOMESTIC, PHASE_SINGLE, 3.0, 5000.0, 3.0, 0.0, 0.0, "2026-01-15", 1
    };
    customer_add_record(&c1);

    /* 2. Standard Domestic */
    Consumer c2 = {
        "VB-1002", "Ananya Verma", "+91 9414098765", "ananya.v@example.com",
        "Villa 12, Malviya Nagar, Jaipur", "MTR-DOM-4401",
        CAT_DOMESTIC, PHASE_SINGLE, 2.0, 3000.0, 0.0, 450.0, 0.0, "2026-02-10", 1
    };
    customer_add_record(&c2);

    /* 3. Commercial Tech Hub */
    Consumer c3 = {
        "VB-1003", "Jaipur Tech Innovation Hub", "+91 141 2789012", "billing@jaipurtechhub.com",
        "Plot 9, Sitapura Industrial Area, Jaipur", "MTR-COM-9923",
        CAT_COMMERCIAL, PHASE_THREE, 25.0, 50000.0, 10.0, 0.0, 1500.0, "2026-03-01", 1
    };
    customer_add_record(&c3);

    /* 4. Industrial Textile Factory */
    Consumer c4 = {
        "VB-1004", "Pink City Textiles & Weaving", "+91 9828033221", "admin@pinkcitytextiles.in",
        "RIICO Industrial Area, Sanganer, Jaipur", "MTR-IND-1002",
        CAT_INDUSTRIAL, PHASE_THREE, 75.0, 150000.0, 0.0, 12450.0, 0.0, "2026-01-05", 1
    };
    customer_add_record(&c4);

    /* 5. Agricultural Farm Pump */
    Consumer c5 = {
        "VB-1005", "Kisan Vikas Tubewell Cooperative", "+91 9602054321", "kisan.vikas@rajagro.org",
        "Survey 412, Bagru Rural, Jaipur", "MTR-AGR-3320",
        CAT_AGRICULTURAL, PHASE_THREE, 10.0, 10000.0, 0.0, 0.0, 0.0, "2026-04-12", 1
    };
    customer_add_record(&c5);

    /* 6. Akshar Miyani (Lead Architect) */
    Consumer c6 = {
        "VB-1006", "Akshar Miyani (MCA MUJ)", "+91 9999988888", "akshar.miyani@muj.manipal.edu",
        "Hostel Block 4, Manipal University Jaipur, Dehmi Kalan", "MTR-DOM-7700",
        CAT_DOMESTIC, PHASE_SINGLE, 5.0, 8000.0, 5.0, 0.0, 2500.0, "2026-01-01", 1
    };
    customer_add_record(&c6);

    /* Historical Billing Records */
    /* VB-1001: 3 Cycles */
    BillBreakdown b1, b2, b3;
    billing_calculate(&c1, 1000.0, 1280.0, 150.0, 1.0, &b1);
    strncpy(b1.billing_cycle, "JUL-2026", 15);
    b1.status = BILL_PAID;
    b1.amount_paid = b1.net_payable_amount;
    billing_add_record(&b1);

    billing_calculate(&c1, 1280.0, 1610.0, 170.0, 1.0, &b2);
    strncpy(b2.billing_cycle, "AUG-2026", 15);
    b2.status = BILL_PAID;
    b2.amount_paid = b2.net_payable_amount;
    billing_add_record(&b2);

    billing_calculate(&c1, 1610.0, 1990.0, 180.0, 1.0, &b3);
    strncpy(b3.billing_cycle, "SEP-2026", 15);
    b3.status = BILL_PENDING;
    billing_add_record(&b3);

    /* VB-1003: Commercial 2 Cycles */
    BillBreakdown b4, b5;
    billing_calculate(&c3, 5000.0, 7400.0, 400.0, 1.0, &b4);
    strncpy(b4.billing_cycle, "AUG-2026", 15);
    b4.status = BILL_PAID;
    b4.amount_paid = b4.net_payable_amount;
    billing_add_record(&b4);

    billing_calculate(&c3, 7400.0, 10150.0, 450.0, 1.0, &b5);
    strncpy(b5.billing_cycle, "SEP-2026", 15);
    b5.status = BILL_PENDING;
    billing_add_record(&b5);

    /* VB-1004: Industrial 2 Cycles with Power Factor 0.88 */
    BillBreakdown b6;
    billing_calculate(&c4, 12000.0, 18500.0, 0.0, 0.88, &b6);
    strncpy(b6.billing_cycle, "SEP-2026", 15);
    b6.status = BILL_PARTIALLY_PAID;
    b6.amount_paid = 25000.0;
    billing_add_record(&b6);

    /* VB-1005: Agricultural */
    BillBreakdown b7;
    billing_calculate(&c5, 2000.0, 2850.0, 0.0, 1.0, &b7);
    strncpy(b7.billing_cycle, "SEP-2026", 15);
    b7.status = BILL_PAID;
    b7.amount_paid = b7.net_payable_amount;
    billing_add_record(&b7);

    /* VB-1006: Akshar Miyani Net Zero Solar Home */
    BillBreakdown b8;
    billing_calculate(&c6, 3000.0, 3320.0, 310.0, 1.0, &b8);
    strncpy(b8.billing_cycle, "SEP-2026", 15);
    b8.status = BILL_PAID;
    b8.amount_paid = b8.net_payable_amount;
    billing_add_record(&b8);

    /* Sample Payments */
    PaymentRecord p1 = {"REC-1001", "BILL-1001", "VB-1001", "2026-07-20 11:20:00", b1.net_payable_amount, PAY_UPI, "UPI-99482710"};
    PaymentRecord p2 = {"REC-1002", "BILL-1002", "VB-1001", "2026-08-18 16:45:00", b2.net_payable_amount, PAY_UPI, "UPI-88129034"};
    PaymentRecord p3 = {"REC-1003", "BILL-1004", "VB-1003", "2026-08-25 14:10:00", b4.net_payable_amount, PAY_NET_BANKING, "HDFC-NETB-44021"};
    PaymentRecord p4 = {"REC-1004", "BILL-1006", "VB-1004", "2026-09-08 10:15:00", 25000.0, PAY_CREDIT_DEBIT_CARD, "POS-ICICI-8812"};
    payment_add_record(&p1);
    payment_add_record(&p2);
    payment_add_record(&p3);
    payment_add_record(&p4);

    storage_save_all();
    storage_export_csv();

    ui_message_box("Demo Seeding Successful", "6 consumers, 8 multi-cycle bills, and payment records created!", 1);
}

void storage_display_stats(void) {
    ui_header("STORAGE & SYSTEM DIAGNOSTICS", "Disk Files, Memory Records, and Ledger Health");

    printf("  " DBOX_TL);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_TR "\n");

    printf("  " DBOX_V "  " CLR_CYAN CLR_BOLD "⚡ IN-MEMORY REPOSITORIES" CLR_RESET "                                           " DBOX_V "\n");
    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    printf("  " DBOX_V "  " CLR_WHITE "Active Consumers in Memory :" CLR_RESET " %-10d / %-10d (Max)            " DBOX_V "\n",
           customer_get_count(), MAX_CONSUMERS);
    printf("  " DBOX_V "  " CLR_WHITE "Billed Invoices in Memory  :" CLR_RESET " %-10d / %-10d (Max)            " DBOX_V "\n",
           billing_get_count(), MAX_BILLS);
    printf("  " DBOX_V "  " CLR_WHITE "Payment Receipts in Memory :" CLR_RESET " %-10d / %-10d (Max)            " DBOX_V "\n",
           payment_get_count(), MAX_PAYMENTS);

    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    printf("  " DBOX_V "  " CLR_CYAN CLR_BOLD "⚡ PERSISTENCE STORAGE FILES" CLR_RESET "                                         " DBOX_V "\n");
    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    printf("  " DBOX_V "  " CLR_GRAY "•" CLR_RESET " " CLR_WHITE "%-24s" CLR_RESET "  " CLR_GREEN "[Binary Datastore]" CLR_RESET "                             " DBOX_V "\n", CONSUMER_DAT);
    printf("  " DBOX_V "  " CLR_GRAY "•" CLR_RESET " " CLR_WHITE "%-24s" CLR_RESET "  " CLR_GREEN "[Binary Datastore]" CLR_RESET "                             " DBOX_V "\n", BILLS_DAT);
    printf("  " DBOX_V "  " CLR_GRAY "•" CLR_RESET " " CLR_WHITE "%-24s" CLR_RESET "  " CLR_GREEN "[Binary Datastore]" CLR_RESET "                             " DBOX_V "\n", PAYMENTS_DAT);
    printf("  " DBOX_V "  " CLR_GRAY "•" CLR_RESET " " CLR_WHITE "data/export_consumers.csv" CLR_RESET "  " CLR_YELLOW "[CSV Spreadsheet]" CLR_RESET "                            " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_GRAY "•" CLR_RESET " " CLR_WHITE "data/export_bills.csv    " CLR_RESET "  " CLR_YELLOW "[CSV Spreadsheet]" CLR_RESET "                            " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_GRAY "•" CLR_RESET " " CLR_WHITE "config/tariffs.cfg       " CLR_RESET "  " CLR_CYAN "[Config Settings]" CLR_RESET "                            " DBOX_V "\n");

    printf("  " DBOX_BL);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_BR "\n");

    pause_prompt();
}
