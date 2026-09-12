/**
 * @file analytics.c
 * @brief Implementation of visual analytics, charts, and carbon footprint.
 * @author Akshar Miyani
 */

#include "analytics.h"
#include "billing.h"
#include "customer.h"
#include "payment.h"
#include "tariff.h"
#include "ui.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

void analytics_draw_barchart(const double *values, const char **labels, int count, double max_val) {
    if (count <= 0 || !values || !labels) return;

    if (max_val <= 0.0) {
        for (int i = 0; i < count; i++) {
            if (values[i] > max_val) max_val = values[i];
        }
    }
    if (max_val <= 0.0) max_val = 1.0;

    int max_bar_width = 38;

    for (int i = 0; i < count; i++) {
        double ratio = values[i] / max_val;
        if (ratio > 1.0) ratio = 1.0;
        int bar_len = (int)(ratio * max_bar_width);

        printf("  " CLR_WHITE "%-14.14s" CLR_RESET " │", labels[i]);
        printf(CLR_CYAN);
        for (int b = 0; b < bar_len; b++) printf("█");
        printf(CLR_DARK_GRAY);
        for (int b = bar_len; b < max_bar_width; b++) printf("░");
        printf(CLR_RESET "  " CLR_YELLOW "%8.1f units" CLR_RESET "\n", values[i]);
    }
}

void analytics_print_carbon_footprint(double total_kwh, double solar_kwh) {
    double co2_gross_kg = total_kwh * 0.82;
    double co2_avoided_kg = solar_kwh * 0.82;
    double co2_net_kg = (total_kwh - solar_kwh) * 0.82;
    if (co2_net_kg < 0.0) co2_net_kg = 0.0;

    double trees_needed = co2_net_kg / 22.0;

    printf("  " DBOX_TL);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_TR "\n");

    printf("  " DBOX_V "  " CLR_GREEN CLR_BOLD "🌱 ENVIRONMENTAL IMPACT & CARBON FOOTPRINT ASSESSMENT" CLR_RESET "                  " DBOX_V "\n");
    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    printf("  " DBOX_V "  " CLR_GRAY "Gross Grid Energy Consumed  :" CLR_RESET " %10.1f kWh                        " DBOX_V "\n", total_kwh);
    printf("  " DBOX_V "  " CLR_GRAY "Clean Solar Energy Exported :" CLR_RESET " " CLR_GREEN "%10.1f kWh" CLR_RESET "                        " DBOX_V "\n", solar_kwh);
    printf("  " DBOX_V "  " CLR_GRAY "Net CO2 Greenhouse Emissions:" CLR_RESET " " CLR_RED "%10.1f kg CO2" CLR_RESET "                     " DBOX_V "\n", co2_net_kg);
    printf("  " DBOX_V "  " CLR_GRAY "Solar Carbon Offset Avoided :" CLR_RESET " " CLR_GREEN "%10.1f kg CO2" CLR_RESET "                     " DBOX_V "\n", co2_avoided_kg);
    printf("  " DBOX_V "  " CLR_GRAY "Trees Needed to Neutralize  :" CLR_RESET " " CLR_YELLOW "%10.1f mature trees/year" CLR_RESET "         " DBOX_V "\n", trees_needed);

    printf("  " DBOX_BL);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_BR "\n\n");
}

void analytics_system_overview(void) {
    ui_header("GRID ANALYTICS & UTILITY INTELLIGENCE", "System-Wide Consumption Metrics, Revenue, and Solar Share");

    int total_consumers = customer_get_count();
    int total_bills = billing_get_count();

    double total_units_billed = 0.0;
    double total_solar_exported = 0.0;
    double total_assessed = 0.0;
    double total_paid = 0.0;
    double total_arrears = 0.0;

    double cat_units[4] = {0.0, 0.0, 0.0, 0.0};

    for (int i = 0; i < total_bills; i++) {
        BillBreakdown *b = billing_get_by_index(i);
        total_units_billed += b->billed_units;
        total_solar_exported += b->solar_units;
        total_assessed += b->current_cycle_total;
        total_paid += b->amount_paid;

        Consumer *c = customer_find_by_id(b->consumer_id);
        if (c && c->category >= 0 && c->category < 4) {
            cat_units[c->category] += b->billed_units;
        }
    }

    for (int i = 0; i < total_consumers; i++) {
        Consumer *c = customer_get_by_index(i);
        total_arrears += c->outstanding_arrears;
    }

    printf("  " DBOX_TL);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_TR "\n");

    printf("  " DBOX_V "  " CLR_CYAN CLR_BOLD "⚡ SYSTEM CAPACITY & REVENUE LEDGER SUMMARY" CLR_RESET "                             " DBOX_V "\n");
    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    printf("  " DBOX_V "  " CLR_WHITE "Registered Consumers :" CLR_RESET " %-12d  " 
           CLR_WHITE "Total Invoices Issued :" CLR_RESET " %-12d " DBOX_V "\n", total_consumers, total_bills);
    printf("  " DBOX_V "  " CLR_WHITE "Total Energy Billed  :" CLR_RESET " %-10.1f kWh  " 
           CLR_WHITE "Total Solar Exported :" CLR_RESET " %-10.1f kWh  " DBOX_V "\n", total_units_billed, total_solar_exported);
    printf("  " DBOX_V "  " CLR_WHITE "Total Gross Assessed :" CLR_RESET " ₹ %-12.2f  " 
           CLR_WHITE "Realized Revenue Paid:" CLR_RESET " ₹ %-12.2f  " DBOX_V "\n", total_assessed, total_paid);
    printf("  " DBOX_V "  " CLR_WHITE "Pending Grid Arrears :" CLR_RESET " " CLR_RED "₹ %-12.2f" CLR_RESET "  " 
           CLR_WHITE "Realization Rate     :" CLR_RESET " " CLR_GREEN "%5.1f%%" CLR_RESET "        " DBOX_V "\n", 
           total_arrears, (total_assessed > 0.0) ? (total_paid / total_assessed) * 100.0 : 0.0);

    printf("  " DBOX_BL);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_BR "\n\n");

    printf("  " CLR_WHITE CLR_BOLD "CONSUMPTION BREAKDOWN BY SECTOR (kWh):" CLR_RESET "\n\n");
    const char *cat_names[] = {"Domestic", "Commercial", "Industrial", "Agricultural"};
    analytics_draw_barchart(cat_units, cat_names, 4, 0.0);
    printf("\n");

    analytics_print_carbon_footprint(total_units_billed + total_solar_exported, total_solar_exported);

    pause_prompt();
}

void analytics_consumer_deepdive(void) {
    ui_header("CONSUMER CONSUMPTION PROFILE & PROJECTIONS", "Historical Energy Trends, Daily Burn Rate & Green Tips");

    printf("  " CLR_WHITE "Enter Consumer ID (e.g., VB-1001): " CLR_RESET);
    char id[ID_LEN];
    get_safe_string(id, sizeof(id));

    Consumer *c = customer_find_by_id(id);
    if (!c) {
        ui_message_box("Not Found", "Consumer ID not found.", 0);
        return;
    }

    customer_render_card(c);

    double history_units[12];
    const char *history_labels[12];
    char label_storage[12][32];
    int count = 0;

    int total_bills = billing_get_count();
    for (int i = 0; i < total_bills && count < 12; i++) {
        BillBreakdown *b = billing_get_by_index(i);
        if (strcmp(b->consumer_id, c->id) == 0) {
            history_units[count] = b->billed_units;
            snprintf(label_storage[count], sizeof(label_storage[count]), "%s", b->billing_cycle);
            history_labels[count] = label_storage[count];
            count++;
        }
    }

    if (count > 0) {
        printf("  " CLR_CYAN CLR_BOLD "Historical Energy Consumption Trend (Billed Units):" CLR_RESET "\n\n");
        analytics_draw_barchart(history_units, history_labels, count, 0.0);
        printf("\n");

        double latest_units = history_units[count - 1];
        double daily_burn = latest_units / 30.0;
        printf("  " CLR_WHITE "• Average Daily Burn Rate : " CLR_YELLOW "%.2f units/day" CLR_RESET "\n", daily_burn);
        printf("  " CLR_WHITE "• Projected Next Month    : " CLR_CYAN "%.1f units (±5%% variation)" CLR_RESET "\n\n", latest_units);

        analytics_print_carbon_footprint(latest_units, c->solar_capacity_kw * 120.0);

        printf("  " CLR_CYAN CLR_BOLD "⚡ INTELLIGENT ENERGY CONSERVATION ADVISORY:" CLR_RESET "\n");
        if (latest_units > 300.0) {
            printf("  " CLR_YELLOW "• High Tier Alert:" CLR_RESET " Your consumption falls in the highest tariff slab (Tier 4+).\n");
            printf("    Upgrading to 5-star inverter split ACs and BLDC smart ceiling fans can reduce bill by ~25%%.\n");
        } else if (latest_units > 150.0) {
            printf("  " CLR_GREEN "• Moderate Usage:" CLR_RESET " You are in Tier 3. Shifting washing machine and water heater loads\n");
            printf("    to solar daylight hours will maximize net metering benefits.\n");
        } else {
            printf("  " CLR_GREEN "• Highly Efficient:" CLR_RESET " Excellent power conservation! You qualify for lowest progressive slab rates.\n");
        }
    } else {
        printf("  " CLR_YELLOW "No billing history recorded yet for this consumer." CLR_RESET "\n");
    }

    pause_prompt();
}
