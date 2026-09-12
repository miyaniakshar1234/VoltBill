/**
 * @file billing.c
 * @brief Implementation of meter reading, slab engine, and invoice rendering.
 * @author Akshar Miyani
 */

#include "billing.h"
#include "customer.h"
#include "tariff.h"
#include "ui.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

static BillBreakdown g_bills[MAX_BILLS];
static int g_bill_count = 0;

void billing_init(void) {
    g_bill_count = 0;
}

int billing_get_count(void) {
    return g_bill_count;
}

BillBreakdown *billing_get_by_index(int index) {
    if (index >= 0 && index < g_bill_count) {
        return &g_bills[index];
    }
    return NULL;
}

BillBreakdown *billing_find_by_id(const char *bill_id) {
    if (!bill_id || bill_id[0] == '\0') return NULL;
    for (int i = 0; i < g_bill_count; i++) {
        if (strcmp(g_bills[i].bill_id, bill_id) == 0) {
            return &g_bills[i];
        }
    }
    return NULL;
}

BillBreakdown *billing_get_latest_for_consumer(const char *consumer_id) {
    if (!consumer_id || consumer_id[0] == '\0') return NULL;
    BillBreakdown *latest = NULL;
    for (int i = 0; i < g_bill_count; i++) {
        if (strcmp(g_bills[i].consumer_id, consumer_id) == 0) {
            latest = &g_bills[i];
        }
    }
    return latest;
}

int billing_add_record(const BillBreakdown *b) {
    if (g_bill_count >= MAX_BILLS || !b) return 0;
    g_bills[g_bill_count++] = *b;
    return 1;
}

void billing_clear_all(void) {
    g_bill_count = 0;
}

void billing_calculate(const Consumer *c, double prev_reading, double curr_reading,
                       double solar_units, double peak_units, double power_factor, BillBreakdown *b) {
    if (!c || !b) return;

    memset(b, 0, sizeof(BillBreakdown));

    char date_str[DATE_LEN], cycle_str[16];
    get_current_date(date_str, sizeof(date_str));
    get_current_billing_cycle(cycle_str, sizeof(cycle_str));

    snprintf(b->bill_id, sizeof(b->bill_id), "BILL-%04d", g_bill_count + 1001);
    strncpy(b->consumer_id, c->id, ID_LEN - 1);
    strncpy(b->bill_date, date_str, DATE_LEN - 1);
    compute_due_date(date_str, 15, b->due_date, sizeof(b->due_date));
    strncpy(b->billing_cycle, cycle_str, sizeof(b->billing_cycle) - 1);

    b->prev_reading = prev_reading;
    b->curr_reading = curr_reading;

    double gross = curr_reading - prev_reading;
    if (gross < 0.0) {
        gross = (100000.0 - prev_reading) + curr_reading;
    }
    b->gross_units = gross;
    b->solar_units = solar_units;

    double net = gross - solar_units;
    if (net < 0.0) net = 0.0;
    b->billed_units = net;
    b->peak_units = peak_units;
    b->power_factor = (power_factor <= 0.0) ? 1.0 : power_factor;

    TariffConfig *t = tariff_get(c->category);

    /* Slab Engine */
    b->slab_breakdown_count = t->slab_count;
    b->total_energy_charges = 0.0;

    double units_remaining = net;
    for (int s = 0; s < t->slab_count; s++) {
        b->slab_rates[s] = t->slabs[s].rate_per_unit;
        double slab_capacity = t->slabs[s].max_units - t->slabs[s].min_units;
        if (s == 0) slab_capacity = t->slabs[s].max_units;

        if (t->slabs[s].max_units >= 999990.0) {
            b->slab_units[s] = units_remaining;
        } else {
            if (units_remaining > slab_capacity) {
                b->slab_units[s] = slab_capacity;
            } else {
                b->slab_units[s] = units_remaining;
            }
        }

        if (b->slab_units[s] < 0.0) b->slab_units[s] = 0.0;
        b->slab_amounts[s] = b->slab_units[s] * b->slab_rates[s];
        b->total_energy_charges += b->slab_amounts[s];

        units_remaining -= b->slab_units[s];
        if (units_remaining <= 0.0) {
            for (int k = s + 1; k < t->slab_count; k++) {
                b->slab_units[k] = 0.0;
                b->slab_rates[k] = t->slabs[k].rate_per_unit;
                b->slab_amounts[k] = 0.0;
            }
            break;
        }
    }

    /* Time-of-Day (ToD) Surcharge calculation */
    b->tod_adjustment = 0.0;
    if (t->tod_peak_surcharge_pct > 0.0 && peak_units > 0.0) {
        double avg_rate = (b->billed_units > 0.0) ? (b->total_energy_charges / b->billed_units) : t->slabs[0].rate_per_unit;
        b->tod_adjustment = (peak_units * avg_rate) * (t->tod_peak_surcharge_pct / 100.0);
    }

    /* Fixed & Ancillary Charges */
    b->fixed_charges = c->sanctioned_load_kw * t->fixed_charge_per_kw;
    b->meter_rent = t->meter_rent;
    b->regulatory_surcharge = b->total_energy_charges * (t->regulatory_surcharge_pct / 100.0);
    b->electricity_duty = (b->total_energy_charges + b->fixed_charges) * (t->electricity_duty_pct / 100.0);
    b->green_cess = b->billed_units * t->green_cess_per_unit;
    b->fppca_charges = b->billed_units * t->fppca_per_unit;

    /* Industrial Power Factor Surcharge / Incentive */
    b->pf_penalty_or_rebate = 0.0;
    if (c->category == CAT_INDUSTRIAL) {
        if (b->power_factor < 0.90) {
            double deficit = (0.90 - b->power_factor) * 100.0;
            b->pf_penalty_or_rebate = b->total_energy_charges * (deficit * 0.02);
        } else if (b->power_factor > 0.95) {
            b->pf_penalty_or_rebate = - (b->total_energy_charges * 0.01);
        }
    }

    b->current_cycle_total = b->total_energy_charges + b->tod_adjustment + b->fixed_charges + b->meter_rent +
                             b->regulatory_surcharge + b->electricity_duty + b->green_cess +
                             b->fppca_charges + b->pf_penalty_or_rebate;

    b->previous_arrears = c->outstanding_arrears;

    /* Advance Credit Adjustment */
    double gross_due = b->current_cycle_total + b->previous_arrears;
    if (c->advance_credit > 0.0) {
        if (c->advance_credit >= gross_due) {
            b->advance_adjusted = gross_due;
            b->net_payable_amount = 0.0;
            b->status = BILL_PAID;
        } else {
            b->advance_adjusted = c->advance_credit;
            b->net_payable_amount = gross_due - c->advance_credit;
            b->status = BILL_PENDING;
        }
    } else {
        b->advance_adjusted = 0.0;
        b->net_payable_amount = gross_due;
        b->status = BILL_PENDING;
    }

    b->prompt_payment_rebate = b->net_payable_amount * (t->prompt_rebate_pct / 100.0);
    b->late_payment_surcharge = b->net_payable_amount * (t->late_penalty_pct / 100.0);
    b->amount_paid = (b->status == BILL_PAID) ? b->current_cycle_total : 0.0;
}

void billing_render_invoice(const BillBreakdown *b, const Consumer *c) {
    if (!b || !c) return;

    printf("\n");
    printf("  " DBOX_TL);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_TR "\n");

    printf("  " DBOX_V "  " CLR_CYAN CLR_BOLD "VOLTBILL STATE UTILITY DISTRIBUTION COMPANY" CLR_RESET "                  " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_GRAY "High-Performance Utility Systems Architecture Engine" CLR_RESET "                " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_VIOLET "Lead Architect: Akshar Miyani" CLR_RESET "                                       " DBOX_V "\n");

    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    printf("  " DBOX_V "  " CLR_YELLOW CLR_BOLD "TAX INVOICE / ELECTRICITY BILL" CLR_RESET "         " 
           CLR_WHITE "Bill No:" CLR_RESET " " CLR_CYAN CLR_BOLD "%-16s" CLR_RESET "      " DBOX_V "\n", b->bill_id);
    printf("  " DBOX_V "  " CLR_GRAY "Billing Cycle: %-12s  Bill Date: %-10s  Due Date: %-10s" CLR_RESET " " DBOX_V "\n",
           b->billing_cycle, b->bill_date, b->due_date);

    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    /* Consumer Info */
    printf("  " DBOX_V "  " CLR_WHITE CLR_BOLD "CONSUMER DETAILS:" CLR_RESET "                                                   " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_GRAY "ID     :" CLR_RESET " " CLR_CYAN "%-12s" CLR_RESET "  " 
           CLR_GRAY "Name    :" CLR_RESET " " CLR_WHITE "%-38.38s" CLR_RESET " " DBOX_V "\n", c->id, c->name);
    printf("  " DBOX_V "  " CLR_GRAY "Meter  :" CLR_RESET " %-12s  " 
           CLR_GRAY "Category:" CLR_RESET " %-38s " DBOX_V "\n", c->meter_no, category_to_string(c->category));
    printf("  " DBOX_V "  " CLR_GRAY "Load   :" CLR_RESET " %-5.2f kW        " 
           CLR_GRAY "Supply  :" CLR_RESET " %-38s " DBOX_V "\n", 
           c->sanctioned_load_kw, c->phase == PHASE_THREE ? "3-Phase (415V)" : "1-Phase (230V)");
    printf("  " DBOX_V "  " CLR_GRAY "Address:" CLR_RESET " %-61.61s " DBOX_V "\n", c->address);

    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    /* Meter Readings */
    printf("  " DBOX_V "  " CLR_WHITE CLR_BOLD "METER CONSUMPTION SUMMARY:" CLR_RESET "                                         " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_GRAY "Previous Index :" CLR_RESET " %10.1f kWh   " 
           CLR_GRAY "Gross Consumed  :" CLR_RESET " %10.1f kWh   " DBOX_V "\n", b->prev_reading, b->gross_units);
    printf("  " DBOX_V "  " CLR_GRAY "Current Index  :" CLR_RESET " %10.1f kWh   " 
           CLR_GRAY "Solar Exported  :" CLR_RESET " %10.1f kWh   " DBOX_V "\n", b->curr_reading, b->solar_units);
    printf("  " DBOX_V "  " CLR_GRAY "Power Factor   :" CLR_RESET " %10.2f       " 
           CLR_GRAY "NET BILLED UNITS:" CLR_RESET " " CLR_GREEN CLR_BOLD "%10.1f kWh" CLR_RESET "   " DBOX_V "\n",
           b->power_factor, b->billed_units);

    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    /* Slabs breakdown */
    printf("  " DBOX_V "  " CLR_WHITE CLR_BOLD "SLAB-BASED ENERGY CHARGES BREAKDOWN:" CLR_RESET "                              " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_GRAY "%-6s  %-24s  %12s  %18s" CLR_RESET "  " DBOX_V "\n", "Slab", "Units Billed", "Rate (₹)", "Amount (₹)");
    printf("  " DBOX_V "  " CLR_DARK_GRAY "──────────────────────────────────────────────────────────────────" CLR_RESET "  " DBOX_V "\n");

    for (int s = 0; s < b->slab_breakdown_count; s++) {
        if (b->slab_units[s] > 0.0 || s == 0) {
            printf("  " DBOX_V "  Tier %-2d %10.1f kWh           @ ₹ %6.2f      ₹ %14.2f  " DBOX_V "\n",
                   s + 1, b->slab_units[s], b->slab_rates[s], b->slab_amounts[s]);
        }
    }
    printf("  " DBOX_V "  " CLR_WHITE CLR_BOLD "Subtotal Energy Charges:" CLR_RESET "                            " 
           CLR_YELLOW CLR_BOLD "₹ %14.2f" CLR_RESET "  " DBOX_V "\n", b->total_energy_charges);

    if (b->tod_adjustment > 0.0) {
        printf("  " DBOX_V "  " CLR_YELLOW "Time-of-Day (ToD) Peak Hours Surcharge:        ₹ %14.2f" CLR_RESET "  " DBOX_V "\n", b->tod_adjustment);
    }

    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    /* Fixed charges and Taxes */
    printf("  " DBOX_V "  " CLR_WHITE CLR_BOLD "FIXED CHARGES & REGULATORY LEVIES:" CLR_RESET "                                " DBOX_V "\n");
    printf("  " DBOX_V "  Fixed Sanctioned Load Charges               :  ₹ %14.2f  " DBOX_V "\n", b->fixed_charges);
    printf("  " DBOX_V "  Meter Hire & Instrument Rent                :  ₹ %14.2f  " DBOX_V "\n", b->meter_rent);
    printf("  " DBOX_V "  Regulatory Asset Surcharge                  :  ₹ %14.2f  " DBOX_V "\n", b->regulatory_surcharge);
    printf("  " DBOX_V "  State Electricity Duty (Govt. Tax)          :  ₹ %14.2f  " DBOX_V "\n", b->electricity_duty);
    printf("  " DBOX_V "  Clean Energy & Environment Cess             :  ₹ %14.2f  " DBOX_V "\n", b->green_cess);
    printf("  " DBOX_V "  Fuel Surcharge Adjustment (FPPCA)           :  ₹ %14.2f  " DBOX_V "\n", b->fppca_charges);

    if (fabs(b->pf_penalty_or_rebate) > 0.01) {
        printf("  " DBOX_V "  Industrial Power Factor Adjustment          :  ₹ %14.2f  " DBOX_V "\n", b->pf_penalty_or_rebate);
    }

    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    /* Total and Adjustments */
    printf("  " DBOX_V "  Current Cycle Assessment                    :  ₹ %14.2f  " DBOX_V "\n", b->current_cycle_total);
    if (b->previous_arrears > 0.0) {
        printf("  " DBOX_V "  " CLR_RED "Add: Previous Outstanding Arrears           :  ₹ %14.2f" CLR_RESET "  " DBOX_V "\n", b->previous_arrears);
    }
    if (b->advance_adjusted > 0.0) {
        printf("  " DBOX_V "  " CLR_GREEN "Less: Advance Payment Adjusted              : -₹ %14.2f" CLR_RESET "  " DBOX_V "\n", b->advance_adjusted);
    }

    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    /* Net Payable Highlight */
    printf("  " DBOX_V "  " BG_CYAN CLR_WHITE CLR_BOLD "  NET PAYABLE AMOUNT (BY DUE DATE)         :  ₹ %14.2f  " CLR_RESET "  " DBOX_V "\n", b->net_payable_amount);
    
    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    printf("  " DBOX_V "  " CLR_GREEN "Early Payment (If paid before %s) : Pay ₹ %-12.2f" CLR_RESET " " DBOX_V "\n", 
           b->due_date, (b->net_payable_amount - b->prompt_payment_rebate > 0.0) ? (b->net_payable_amount - b->prompt_payment_rebate) : 0.0);
    printf("  " DBOX_V "  " CLR_RED "Late Surcharge (If paid after %s)  : Pay ₹ %-12.2f" CLR_RESET " " DBOX_V "\n", 
           b->due_date, b->net_payable_amount + b->late_payment_surcharge);

    /* QR Code Placeholder */
    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    printf("  " DBOX_V "  " CLR_DARK_GRAY "[ ▄█▀█▄ UPI DIGITAL PAYMENT ENABLED - SCAN VIA ANY BANKING APP ▄█▀█▄ ]" CLR_RESET "   " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_GRAY "Payment Modes Accepted: UPI, NetBanking, Credit/Debit Cards, Cash Kiosk" CLR_RESET "   " DBOX_V "\n");

    printf("  " DBOX_BL);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_BR "\n");

    printf("  " CLR_GRAY "Generated by VoltBill Engine • Architect: Akshar Miyani" CLR_RESET "\n\n");
}

int billing_export_text_invoice(const BillBreakdown *b, const Consumer *c) {
    if (!b || !c) return 0;

    ensure_directory("data");
    ensure_directory("data/bills");

    char filepath[128];
    snprintf(filepath, sizeof(filepath), "data/bills/%s.txt", b->bill_id);

    FILE *fp = fopen(filepath, "w");
    if (!fp) return 0;

    fprintf(fp, "==========================================================================\n");
    fprintf(fp, "               VOLTBILL STATE UTILITY DISTRIBUTION COMPANY                \n");
    fprintf(fp, "               High-Performance Systems Engineering Engine               \n");
    fprintf(fp, "                       Lead Architect: Akshar Miyani                      \n");
    fprintf(fp, "==========================================================================\n");
    fprintf(fp, "TAX INVOICE / ELECTRICITY BILL                     Bill No: %s\n", b->bill_id);
    fprintf(fp, "Billing Cycle: %-10s  Bill Date: %-10s  Due Date: %-10s\n", b->billing_cycle, b->bill_date, b->due_date);
    fprintf(fp, "--------------------------------------------------------------------------\n");
    fprintf(fp, "CONSUMER DETAILS:\n");
    fprintf(fp, "  ID       : %s\n", c->id);
    fprintf(fp, "  Name     : %s\n", c->name);
    fprintf(fp, "  Meter No : %s\n", c->meter_no);
    fprintf(fp, "  Category : %s\n", category_to_string(c->category));
    fprintf(fp, "  Load     : %.2f kW\n", c->sanctioned_load_kw);
    fprintf(fp, "  Address  : %s\n", c->address);
    fprintf(fp, "--------------------------------------------------------------------------\n");
    fprintf(fp, "METER CONSUMPTION SUMMARY:\n");
    fprintf(fp, "  Previous Meter Index : %10.1f kWh\n", b->prev_reading);
    fprintf(fp, "  Current Meter Index  : %10.1f kWh\n", b->curr_reading);
    fprintf(fp, "  Gross Units Consumed : %10.1f kWh\n", b->gross_units);
    fprintf(fp, "  Solar Net Offset     : %10.1f kWh\n", b->solar_units);
    fprintf(fp, "  NET BILLED UNITS     : %10.1f kWh\n", b->billed_units);
    fprintf(fp, "  Power Factor         : %10.2f\n", b->power_factor);
    fprintf(fp, "--------------------------------------------------------------------------\n");
    fprintf(fp, "SLAB-BASED CHARGES:\n");
    for (int s = 0; s < b->slab_breakdown_count; s++) {
        if (b->slab_units[s] > 0.0 || s == 0) {
            fprintf(fp, "  Tier %-2d: %8.1f units @ Rs. %5.2f = Rs. %10.2f\n",
                    s + 1, b->slab_units[s], b->slab_rates[s], b->slab_amounts[s]);
        }
    }
    fprintf(fp, "  Total Energy Charges             : Rs. %10.2f\n", b->total_energy_charges);
    if (b->tod_adjustment > 0.0) {
        fprintf(fp, "  Time-of-Day (ToD) Peak Surcharge : Rs. %10.2f\n", b->tod_adjustment);
    }
    fprintf(fp, "--------------------------------------------------------------------------\n");
    fprintf(fp, "FIXED CHARGES & LEVIES:\n");
    fprintf(fp, "  Fixed Contract Demand Charge     : Rs. %10.2f\n", b->fixed_charges);
    fprintf(fp, "  Meter Hire Rent                  : Rs. %10.2f\n", b->meter_rent);
    fprintf(fp, "  Regulatory Asset Surcharge       : Rs. %10.2f\n", b->regulatory_surcharge);
    fprintf(fp, "  State Electricity Duty (Tax)     : Rs. %10.2f\n", b->electricity_duty);
    fprintf(fp, "  Clean Energy Cess                : Rs. %10.2f\n", b->green_cess);
    fprintf(fp, "  Fuel Price Adjustment (FPPCA)    : Rs. %10.2f\n", b->fppca_charges);
    if (fabs(b->pf_penalty_or_rebate) > 0.01) {
        fprintf(fp, "  Industrial PF Adjustment         : Rs. %10.2f\n", b->pf_penalty_or_rebate);
    }
    fprintf(fp, "--------------------------------------------------------------------------\n");
    fprintf(fp, "ACCOUNT SUMMARY:\n");
    fprintf(fp, "  Current Cycle Assessment         : Rs. %10.2f\n", b->current_cycle_total);
    fprintf(fp, "  Previous Arrears                 : Rs. %10.2f\n", b->previous_arrears);
    fprintf(fp, "  Advance Adjusted                 :-Rs. %10.2f\n", b->advance_adjusted);
    fprintf(fp, "  NET AMOUNT PAYABLE BY DUE DATE   : Rs. %10.2f\n", b->net_payable_amount);
    fprintf(fp, "--------------------------------------------------------------------------\n");
    fprintf(fp, "  Prompt Payment (Before Due Date) : Pay Rs. %.2f\n", 
            (b->net_payable_amount - b->prompt_payment_rebate > 0.0) ? (b->net_payable_amount - b->prompt_payment_rebate) : 0.0);
    fprintf(fp, "  Late Payment (After Due Date)    : Pay Rs. %.2f\n", b->net_payable_amount + b->late_payment_surcharge);
    fprintf(fp, "==========================================================================\n");
    fprintf(fp, "VoltBill Engine | Engineered by Akshar Miyani\n");

    fclose(fp);
    return 1;
}

int billing_quick_bill(const char *consumer_id, double curr_reading) {
    Consumer *c = customer_find_by_id(consumer_id);
    if (!c) {
        printf("VoltBill: Consumer '%s' not found.\n", consumer_id);
        return 0;
    }

    BillBreakdown *latest = billing_get_latest_for_consumer(c->id);
    double prev_reading = latest ? latest->curr_reading : 0.0;
    double solar = c->solar_capacity_kw * 40.0;
    double peak = (c->category == CAT_COMMERCIAL || c->category == CAT_INDUSTRIAL) ? 50.0 : 0.0;
    double pf = 1.0;

    BillBreakdown b;
    billing_calculate(c, prev_reading, curr_reading, solar, peak, pf, &b);
    c->outstanding_arrears = b.net_payable_amount;
    billing_add_record(&b);
    billing_export_text_invoice(&b, c);

    extern int storage_save_all(void);
    storage_save_all();

    char audit_desc[128];
    snprintf(audit_desc, sizeof(audit_desc), "CLI Generated %s for %s (Rs. %.2f)", b.bill_id, c->id, b.net_payable_amount);
    audit_log("CLI_BILL_GENERATE", audit_desc);

    printf("VoltBill: Bill %s created for %s | Net Payable: Rs. %.2f | Due: %s\n",
           b.bill_id, c->id, b.net_payable_amount, b.due_date);
    return 1;
}

void billing_generate_flow(void) {
    ui_header("METER READING & BILL GENERATION", "Compute Slab Charges, Levies, and Issue Electricity Invoice");

    if (customer_get_count() == 0) {
        ui_message_box("No Consumers", "Please register a customer first before generating bills.", 0);
        return;
    }

    printf("  " CLR_WHITE "Enter Consumer ID (e.g., VB-1001) or Meter Number: " CLR_RESET);
    char query[64];
    get_safe_string(query, sizeof(query));

    Consumer *c = customer_find_by_id(query);
    if (!c) c = customer_find_by_meter(query);

    if (!c) {
        ui_message_box("Not Found", "Consumer could not be found with that ID or Meter.", 0);
        return;
    }

    if (!c->is_active) {
        ui_message_box("Inactive Connection", "This connection is disconnected or inactive.", 0);
        return;
    }

    customer_render_card(c);

    BillBreakdown *latest = billing_get_latest_for_consumer(c->id);
    double prev_reading = latest ? latest->curr_reading : 0.0;

    printf("  " CLR_GRAY "Recorded Previous Reading:" CLR_RESET " " CLR_CYAN "%.1f kWh" CLR_RESET "\n", prev_reading);
    
    int change_prev = get_safe_int("  Use this previous reading? [1 = Yes, 0 = Enter Custom]: ", 0, 1);
    if (!change_prev) {
        prev_reading = get_safe_double("  Enter Custom Previous Reading: ", 0.0, 9999999.0);
    }

    double curr_reading = 0.0;
    while (1) {
        curr_reading = get_safe_double("  Enter Current Meter Reading (kWh): ", 0.0, 9999999.0);
        if (curr_reading < prev_reading) {
            printf("\n  " CLR_YELLOW "⚠️ Notice: Current reading (%.1f) is LESS than previous reading (%.1f)." CLR_RESET "\n",
                   curr_reading, prev_reading);
            printf("    [1] Meter Dial Rollover (exceeded 99999 and reset to 0)\n");
            printf("    [2] Defective / Faulty Meter Replacement\n");
            printf("    [3] Re-enter Current Reading\n");
            int fix = get_safe_int("    Choose option [1-3]: ", 1, 3);
            if (fix == 3) continue;
        }
        break;
    }

    double solar_units = 0.0;
    if (c->solar_capacity_kw > 0.0) {
        printf("  " CLR_GREEN "Consumer has %.1f kW Rooftop Solar Net Metering." CLR_RESET "\n", c->solar_capacity_kw);
        solar_units = get_safe_double("  Enter Solar Units Exported to Grid (kWh): ", 0.0, 50000.0);
    }

    double peak_units = 0.0;
    TariffConfig *t = tariff_get(c->category);
    if (t->tod_peak_surcharge_pct > 0.0) {
        peak_units = get_safe_double("  Enter Peak-Hour Units (18:00 - 22:00) Consumed: ", 0.0, 100000.0);
    }

    double power_factor = 1.0;
    if (c->category == CAT_INDUSTRIAL) {
        power_factor = get_safe_double("  Enter Recorded Power Factor [0.50 - 1.00]: ", 0.50, 1.00);
    }

    BillBreakdown new_bill;
    billing_calculate(c, prev_reading, curr_reading, solar_units, peak_units, power_factor, &new_bill);

    c->outstanding_arrears = new_bill.net_payable_amount;
    if (new_bill.advance_adjusted > 0.0) {
        c->advance_credit -= new_bill.advance_adjusted;
        if (c->advance_credit < 0.0) c->advance_credit = 0.0;
    }

    billing_add_record(&new_bill);
    billing_export_text_invoice(&new_bill, c);

    extern int storage_save_all(void);
    storage_save_all();

    char audit_desc[128];
    snprintf(audit_desc, sizeof(audit_desc), "Generated %s for %s (Billed: %.1f kWh, Rs. %.2f)",
             new_bill.bill_id, c->id, new_bill.billed_units, new_bill.net_payable_amount);
    audit_log("BILL_GENERATE", audit_desc);

    clear_screen();
    billing_render_invoice(&new_bill, c);

    printf("  " CLR_GREEN "✓ Bill %s generated and exported to data/bills/%s.txt" CLR_RESET "\n", 
           new_bill.bill_id, new_bill.bill_id);
    pause_prompt();
}

void billing_batch_generate_flow(void) {
    ui_header("BATCH GRID BILLING ENGINE", "Run Simultaneous Monthly Assessment for All Active Consumers");

    int consumer_count = customer_get_count();
    if (consumer_count == 0) {
        ui_message_box("Empty Grid", "No consumers registered in database to bill.", 0);
        return;
    }

    printf("  " CLR_WHITE "Active Consumers to Process : " CLR_CYAN CLR_BOLD "%d" CLR_RESET "\n", consumer_count);
    printf("  " CLR_WHITE "Current Billing Cycle       : " CLR_YELLOW);
    char cycle[16];
    get_current_billing_cycle(cycle, sizeof(cycle));
    printf("%s" CLR_RESET "\n\n", cycle);

    int confirm = get_safe_int("  Execute automated grid billing cycle? [1 = Proceed, 0 = Cancel]: ", 0, 1);
    if (!confirm) return;

    printf("\n  " CLR_CYAN "⚡ Processing Batch Invoices:" CLR_RESET "\n");

    int generated = 0;
    double total_units_batched = 0.0;
    double total_revenue_batched = 0.0;

    for (int i = 0; i < consumer_count; i++) {
        Consumer *c = customer_get_by_index(i);
        if (!c || !c->is_active) continue;

        BillBreakdown *latest = billing_get_latest_for_consumer(c->id);
        double prev = latest ? latest->curr_reading : 0.0;
        
        /* Realistic consumption simulation based on sanctioned load */
        double delta = c->sanctioned_load_kw * (80.0 + (rand() % 40));
        double curr = prev + delta;
        double solar = (c->solar_capacity_kw > 0.0) ? (c->solar_capacity_kw * (30.0 + (rand() % 15))) : 0.0;
        double peak = delta * 0.25;
        double pf = (c->category == CAT_INDUSTRIAL) ? 0.91 : 1.0;

        BillBreakdown b;
        billing_calculate(c, prev, curr, solar, peak, pf, &b);
        c->outstanding_arrears = b.net_payable_amount;
        billing_add_record(&b);
        billing_export_text_invoice(&b, c);

        total_units_batched += b.billed_units;
        total_revenue_batched += b.net_payable_amount;
        generated++;

        printf("  ");
        ui_progress_bar(((double)(i + 1) / consumer_count) * 100.0, 30);
        printf("  Processed: %s (%s)\r", c->id, c->name);
        fflush(stdout);
        sleep_ms(30);
    }

    extern int storage_save_all(void);
    storage_save_all();

    char audit_desc[128];
    snprintf(audit_desc, sizeof(audit_desc), "Batch Billed %d consumers (Total: Rs. %.2f)", generated, total_revenue_batched);
    audit_log("BATCH_BILLING_RUN", audit_desc);

    printf("\n\n");
    printf("  " DBOX_TL);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_TR "\n");

    printf("  " DBOX_V "  " CLR_GREEN CLR_BOLD "✓ BATCH BILLING RUN COMPLETED SUCCESSFULLY" CLR_RESET "                             " DBOX_V "\n");
    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    printf("  " DBOX_V "  " CLR_WHITE "Total Invoices Generated :" CLR_RESET " " CLR_CYAN "%-44d" CLR_RESET " " DBOX_V "\n", generated);
    printf("  " DBOX_V "  " CLR_WHITE "Total Energy Billed      :" CLR_RESET " " CLR_YELLOW "%-10.1f kWh" CLR_RESET "                                   " DBOX_V "\n", total_units_batched);
    printf("  " DBOX_V "  " CLR_WHITE "Total Revenue Assessed   :" CLR_RESET " " CLR_GREEN "₹ %-12.2f" CLR_RESET "                                  " DBOX_V "\n", total_revenue_batched);

    printf("  " DBOX_BL);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_BR "\n");

    pause_prompt();
}

void billing_filter_flow(void) {
    ui_header("INVOICE FILTER & AUDIT QUERY ENGINE", "Filter Invoices by Payment Status, Consumer, or Value");

    if (g_bill_count == 0) {
        ui_message_box("No Invoices", "No bills have been generated yet.", 0);
        return;
    }

    const char *filter_opts[] = {
        "Show Pending Unpaid Bills Only",
        "Show Fully Paid Bills Only",
        "Show Partially Paid Bills Only",
        "Show Bills for a Specific Consumer",
        "Show High-Value Bills (> ₹10,000)"
    };

    int sel = ui_menu("FILTER CRITERIA", filter_opts, 5, 0);
    if (sel < 0) return;

    char target_consumer[ID_LEN] = {0};
    if (sel == 3) {
        printf("  Enter Consumer ID (e.g. VB-1001): ");
        get_safe_string(target_consumer, sizeof(target_consumer));
    }

    ui_header("FILTERED INVOICE RESULTS", "Matching Billing Ledgers");

    printf("  " CLR_GRAY "┌──────────────┬────────────┬──────────┬──────────┬──────────────┬──────────────┬───────────┐" CLR_RESET "\n");
    printf("  " CLR_GRAY "│ " CLR_CYAN CLR_BOLD "%-12s" CLR_RESET CLR_GRAY "│ " 
           CLR_WHITE CLR_BOLD "%-10s" CLR_RESET CLR_GRAY "│ " 
           CLR_YELLOW CLR_BOLD "%-8s" CLR_RESET CLR_GRAY "│ " 
           CLR_WHITE CLR_BOLD "%-8s" CLR_RESET CLR_GRAY "│ " 
           CLR_WHITE CLR_BOLD "%-12s" CLR_RESET CLR_GRAY "│ " 
           CLR_GREEN CLR_BOLD "%-12s" CLR_RESET CLR_GRAY "│ " 
           CLR_VIOLET CLR_BOLD "%-9s" CLR_RESET CLR_GRAY "│" CLR_RESET "\n",
           "Bill ID", "Consumer", "Cycle", "Units", "Assessed (₹)", "Payable (₹)", "Status");
    printf("  " CLR_GRAY "├──────────────┼────────────┼──────────┼──────────┼──────────────┼──────────────┼───────────┤" CLR_RESET "\n");

    int match_count = 0;
    for (int i = 0; i < g_bill_count; i++) {
        BillBreakdown *b = &g_bills[i];
        int match = 0;

        if (sel == 0 && b->status == BILL_PENDING) match = 1;
        else if (sel == 1 && b->status == BILL_PAID) match = 1;
        else if (sel == 2 && b->status == BILL_PARTIALLY_PAID) match = 1;
        else if (sel == 3 && strcmp(b->consumer_id, target_consumer) == 0) match = 1;
        else if (sel == 4 && b->net_payable_amount >= 10000.0) match = 1;

        if (match) {
            match_count++;
            const char *st_clr = (b->status == BILL_PAID) ? CLR_GREEN : CLR_YELLOW;
            const char *st_str = (b->status == BILL_PAID) ? "Paid" : (b->status == BILL_PARTIALLY_PAID ? "Partial" : "Pending");

            printf("  " CLR_GRAY "│ " CLR_CYAN "%-12s" CLR_RESET CLR_GRAY "│ " 
                   CLR_WHITE "%-10s" CLR_RESET CLR_GRAY "│ " 
                   CLR_GRAY "%-8s" CLR_RESET CLR_GRAY "│ " 
                   CLR_WHITE "%8.1f" CLR_RESET CLR_GRAY "│ " 
                   CLR_WHITE "%12.2f" CLR_RESET CLR_GRAY "│ " 
                   CLR_YELLOW CLR_BOLD "%12.2f" CLR_RESET CLR_GRAY "│ " 
                   "%s%-9s" CLR_RESET CLR_GRAY "│" CLR_RESET "\n",
                   b->bill_id, b->consumer_id, b->billing_cycle, b->billed_units,
                   b->current_cycle_total, b->net_payable_amount, st_clr, st_str);
        }
    }

    printf("  " CLR_GRAY "└──────────────┴────────────┴──────────┴──────────┴──────────────┴──────────────┴───────────┘" CLR_RESET "\n");
    printf("  " CLR_GRAY "Matching Records Found: " CLR_CYAN "%d" CLR_RESET "\n", match_count);

    pause_prompt();
}

void billing_list_all(void) {
    ui_header("ISSUED BILLS & BILLING HISTORY", "Log of All Electricity Invoices Generated");

    if (g_bill_count == 0) {
        printf("  " CLR_YELLOW "No bills have been generated yet." CLR_RESET "\n");
        pause_prompt();
        return;
    }

    printf("  " CLR_GRAY "┌──────────────┬────────────┬──────────┬──────────┬──────────────┬──────────────┬───────────┐" CLR_RESET "\n");
    printf("  " CLR_GRAY "│ " CLR_CYAN CLR_BOLD "%-12s" CLR_RESET CLR_GRAY "│ " 
           CLR_WHITE CLR_BOLD "%-10s" CLR_RESET CLR_GRAY "│ " 
           CLR_YELLOW CLR_BOLD "%-8s" CLR_RESET CLR_GRAY "│ " 
           CLR_WHITE CLR_BOLD "%-8s" CLR_RESET CLR_GRAY "│ " 
           CLR_WHITE CLR_BOLD "%-12s" CLR_RESET CLR_GRAY "│ " 
           CLR_GREEN CLR_BOLD "%-12s" CLR_RESET CLR_GRAY "│ " 
           CLR_VIOLET CLR_BOLD "%-9s" CLR_RESET CLR_GRAY "│" CLR_RESET "\n",
           "Bill ID", "Consumer", "Cycle", "Units", "Assessed (₹)", "Payable (₹)", "Status");
    printf("  " CLR_GRAY "├──────────────┼────────────┼──────────┼──────────┼──────────────┼──────────────┼───────────┤" CLR_RESET "\n");

    for (int i = 0; i < g_bill_count; i++) {
        BillBreakdown *b = &g_bills[i];
        const char *status_str = "Pending";
        const char *status_clr = CLR_YELLOW;
        if (b->status == BILL_PAID) {
            status_str = "Paid";
            status_clr = CLR_GREEN;
        } else if (b->status == BILL_PARTIALLY_PAID) {
            status_str = "Partial";
            status_clr = CLR_CYAN;
        } else if (b->status == BILL_OVERDUE || is_date_overdue(b->due_date)) {
            status_str = "Overdue";
            status_clr = CLR_RED;
        }

        printf("  " CLR_GRAY "│ " CLR_CYAN "%-12s" CLR_RESET CLR_GRAY "│ " 
               CLR_WHITE "%-10s" CLR_RESET CLR_GRAY "│ " 
               CLR_GRAY "%-8s" CLR_RESET CLR_GRAY "│ " 
               CLR_WHITE "%8.1f" CLR_RESET CLR_GRAY "│ " 
               CLR_WHITE "%12.2f" CLR_RESET CLR_GRAY "│ " 
               CLR_YELLOW CLR_BOLD "%12.2f" CLR_RESET CLR_GRAY "│ " 
               "%s%-9s" CLR_RESET CLR_GRAY "│" CLR_RESET "\n",
               b->bill_id, b->consumer_id, b->billing_cycle, b->billed_units,
               b->current_cycle_total, b->net_payable_amount, status_clr, status_str);
    }

    printf("  " CLR_GRAY "└──────────────┴────────────┴──────────┴──────────┴──────────────┴──────────────┴───────────┘" CLR_RESET "\n");
    printf("  " CLR_GRAY "Total Invoices: " CLR_CYAN CLR_BOLD "%d" CLR_RESET "\n", g_bill_count);

    printf("\n  " CLR_WHITE "Enter Bill ID to view full invoice (or press Enter to return): " CLR_RESET);
    char inspect_id[32];
    get_safe_string(inspect_id, sizeof(inspect_id));
    if (inspect_id[0] != '\0') {
        BillBreakdown *found_bill = billing_find_by_id(inspect_id);
        if (found_bill) {
            Consumer *c = customer_find_by_id(found_bill->consumer_id);
            clear_screen();
            billing_render_invoice(found_bill, c);
            pause_prompt();
        } else {
            ui_message_box("Not Found", "Bill ID was not found in system.", 0);
        }
    }
}
