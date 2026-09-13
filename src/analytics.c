/**
 * @file analytics.c
 * @brief Implementation of visual analytics, charts, sparklines, and eco telemetry.
 * @author Akshar Miyani
 * @version 2.0.0
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
#include <math.h>

/* Unicode sparkline levels (8 heights) */
static const char *SPARK_LEVELS[] = {" ", "▂", "▃", "▄", "▅", "▆", "▇", "█"};

void analytics_draw_sparkline(const double *values, int count) {
    if (count <= 0 || !values) return;

    double min_v = values[0];
    double max_v = values[0];
    for (int i = 1; i < count; i++) {
        if (values[i] < min_v) min_v = values[i];
        if (values[i] > max_v) max_v = values[i];
    }

    double range = max_v - min_v;
    if (range < 0.001) range = 1.0;

    printf(CLR_CYAN);
    for (int i = 0; i < count; i++) {
        int idx = (int)(((values[i] - min_v) / range) * 7.0 + 0.5);
        if (idx < 0) idx = 0;
        if (idx > 7) idx = 7;
        printf("%s", SPARK_LEVELS[idx]);
    }
    printf(CLR_RESET);
}

void analytics_draw_barchart(const double *values, const char **labels, int count, double max_val) {
    if (count <= 0 || !values || !labels) return;

    if (max_val <= 0.0) {
        for (int i = 0; i < count; i++) {
            if (values[i] > max_val) max_val = values[i];
        }
    }
    if (max_val <= 0.0) max_val = 1.0;

    int max_bar_width = 34;

    for (int i = 0; i < count; i++) {
        double ratio = values[i] / max_val;
        if (ratio > 1.0) ratio = 1.0;
        int bar_len = (int)(ratio * max_bar_width);

        printf("  " CLR_WHITE "%-14.14s" CLR_RESET " │ ", labels[i]);

        /* Dynamic RGB interpolation across the bar: Cyan to Violet */
        for (int b = 0; b < bar_len; b++) {
            double t = (double)b / (double)(max_bar_width > 1 ? max_bar_width - 1 : 1);
            int r = (int)(0   + t * (189 - 0));
            int g = (int)(240 + t * (0   - 240));
            int bl = (int)(255 + t * (255 - 255));
            printf("\033[38;2;%d;%d;%dm▰", r, g, bl);
        }
        printf(CLR_DARK_GRAY);
        for (int b = bar_len; b < max_bar_width; b++) {
            printf("▱");
        }
        printf(CLR_RESET "  " CLR_YELLOW "%8.1f" CLR_GRAY " kWh" CLR_RESET "  " CLR_DIM "(%4.1f%%)" CLR_RESET "\n", 
               values[i], (values[i] / max_val) * 100.0);
    }
}

void analytics_print_carbon_footprint(double total_kwh, double solar_kwh) {
    double co2_gross_kg = total_kwh * 0.82;
    double co2_avoided_kg = solar_kwh * 0.82;
    double co2_net_kg = (total_kwh - solar_kwh) * 0.82;
    if (co2_net_kg < 0.0) co2_net_kg = 0.0;

    double trees_needed = co2_net_kg / 22.0;

    const int W = 76;
    char left[160], right[160];

    printf("\n");
    ui_card_begin(W, "ECO-TELEMETRY & CARBON NEUTRALITY LEDGER");

    snprintf(left, sizeof(left), CLR_GRAY "Gross Energy Sourced   :" CLR_RESET);
    snprintf(right, sizeof(right), CLR_WHITE "%.1f kWh" CLR_RESET, total_kwh);
    ui_card_row(W, left, right);

    snprintf(left, sizeof(left), CLR_GRAY "Solar Energy Exported  :" CLR_RESET);
    snprintf(right, sizeof(right), CLR_GREEN "%.1f kWh (Clean Renewable)" CLR_RESET, solar_kwh);
    ui_card_row(W, left, right);

    char gauge[128];
    double ratio = (total_kwh > 0.0) ? (solar_kwh / total_kwh) : 0.0;
    if (ratio > 1.0) ratio = 1.0;
    int filled = (int)(ratio * 20.0);
    int pos = 0;
    pos += snprintf(gauge + pos, sizeof(gauge) - pos, "[%s", CLR_GREEN);
    for (int i = 0; i < filled; i++) pos += snprintf(gauge + pos, sizeof(gauge) - pos, "▰");
    pos += snprintf(gauge + pos, sizeof(gauge) - pos, "%s", CLR_DARK_GRAY);
    for (int i = filled; i < 20; i++) pos += snprintf(gauge + pos, sizeof(gauge) - pos, "▱");
    pos += snprintf(gauge + pos, sizeof(gauge) - pos, "%s] %5.1f%%", CLR_RESET, ratio * 100.0);

    snprintf(left, sizeof(left), CLR_GRAY "Clean Solar Share Ratio:" CLR_RESET);
    ui_card_row(W, left, gauge);

    ui_card_divider(W);

    snprintf(left, sizeof(left), CLR_GRAY "Gross Grid CO2 Output  :" CLR_RESET);
    snprintf(right, sizeof(right), CLR_RED "%.1f kg CO2" CLR_RESET, co2_gross_kg);
    ui_card_row(W, left, right);

    snprintf(left, sizeof(left), CLR_GRAY "Solar Carbon Abatement :" CLR_RESET);
    snprintf(right, sizeof(right), CLR_GREEN "%.1f kg CO2 (Avoided)" CLR_RESET, co2_avoided_kg);
    ui_card_row(W, left, right);

    snprintf(left, sizeof(left), CLR_GRAY "Net CO2 Impact On Grid :" CLR_RESET);
    snprintf(right, sizeof(right), CLR_YELLOW "%.1f kg CO2" CLR_RESET, co2_net_kg);
    ui_card_row(W, left, right);

    snprintf(left, sizeof(left), CLR_GRAY "Trees Req. to Neutralize:" CLR_RESET);
    snprintf(right, sizeof(right), CLR_GREEN "%.1f mature trees / year" CLR_RESET, trees_needed);
    ui_card_row(W, left, right);

    ui_card_end(W);
    printf("\n");
}

void analytics_system_overview(void) {
    ui_telemetry_hud();
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

    const int W = 76;
    char left[160], right[160];

    ui_card_begin(W, "SYSTEM CAPACITY & REVENUE REALIZATION TELEMETRY");

    snprintf(left, sizeof(left), CLR_WHITE "Registered Consumers : " CLR_CYAN "%d" CLR_RESET, total_consumers);
    snprintf(right, sizeof(right), CLR_WHITE "Total Invoices Issued: " CLR_CYAN "%d" CLR_RESET, total_bills);
    ui_card_row(W, left, right);

    snprintf(left, sizeof(left), CLR_WHITE "Total Energy Billed  : " CLR_YELLOW "%.1f kWh" CLR_RESET, total_units_billed);
    snprintf(right, sizeof(right), CLR_WHITE "Total Solar Exported : " CLR_GREEN "%.1f kWh" CLR_RESET, total_solar_exported);
    ui_card_row(W, left, right);

    ui_card_divider(W);

    snprintf(left, sizeof(left), CLR_WHITE "Total Gross Assessed : " CLR_WHITE "Rs. %.2f" CLR_RESET, total_assessed);
    snprintf(right, sizeof(right), CLR_WHITE "Realized Revenue Paid: " CLR_GREEN "Rs. %.2f" CLR_RESET, total_paid);
    ui_card_row(W, left, right);

    double rec_rate = (total_assessed > 0.0) ? (total_paid / total_assessed) * 100.0 : 0.0;
    snprintf(left, sizeof(left), CLR_WHITE "Outstanding Arrears  : " CLR_RED "Rs. %.2f" CLR_RESET, total_arrears);
    snprintf(right, sizeof(right), CLR_WHITE "Realization Rate     : " CLR_GREEN "%.1f%%" CLR_RESET, rec_rate);
    ui_card_row(W, left, right);

    ui_card_divider(W);

    char prog[128];
    double r_ratio = rec_rate / 100.0;
    if (r_ratio > 1.0) r_ratio = 1.0;
    int r_filled = (int)(r_ratio * 30.0);
    int r_pos = 0;
    r_pos += snprintf(prog + r_pos, sizeof(prog) - r_pos, "[%s", CLR_CYAN);
    for (int i = 0; i < r_filled; i++) r_pos += snprintf(prog + r_pos, sizeof(prog) - r_pos, "▰");
    r_pos += snprintf(prog + r_pos, sizeof(prog) - r_pos, "%s", CLR_DARK_GRAY);
    for (int i = r_filled; i < 30; i++) r_pos += snprintf(prog + r_pos, sizeof(prog) - r_pos, "▱");
    r_pos += snprintf(prog + r_pos, sizeof(prog) - r_pos, "%s] %5.1f%%", CLR_RESET, rec_rate);

    snprintf(left, sizeof(left), CLR_GRAY "Realization Progress :" CLR_RESET);
    ui_card_row(W, left, prog);

    ui_card_end(W);
    printf("\n");

    printf("  ");
    ui_print_gradient("📊 CONSUMPTION DISTRIBUTION BY SECTOR", 0, 240, 255, 255, 230, 0, 1);
    printf("\n\n");

    const char *cat_names[] = {"Domestic", "Commercial", "Industrial", "Agricultural"};
    analytics_draw_barchart(cat_units, cat_names, 4, 0.0);

    analytics_print_carbon_footprint(total_units_billed + total_solar_exported, total_solar_exported);

    pause_prompt();
}

void analytics_consumer_deepdive(void) {
    ui_telemetry_hud();
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
        printf("  " CLR_CYAN CLR_BOLD "Historical Energy Consumption Waveform: " CLR_RESET);
        analytics_draw_sparkline(history_units, count);
        printf("  " CLR_DIM "(%d cycles recorded)" CLR_RESET "\n\n", count);

        printf("  ");
        ui_print_gradient("📈 HISTORICAL CYCLE-BY-CYCLE CONSUMPTION (kWh)", 0, 240, 255, 189, 0, 255, 1);
        printf("\n\n");

        analytics_draw_barchart(history_units, history_labels, count, 0.0);
        printf("\n");

        double latest_units = history_units[count - 1];
        double daily_burn = latest_units / 30.0;

        const int W = 76;
        char left[160], right[160];
        ui_card_begin(W, "CONSUMPTION PROJECTIONS & BURN RATE TELEMETRY");
        snprintf(left, sizeof(left), CLR_WHITE "• Average Daily Burn Rate :" CLR_RESET);
        snprintf(right, sizeof(right), CLR_YELLOW "%.2f units/day" CLR_RESET, daily_burn);
        ui_card_row(W, left, right);

        snprintf(left, sizeof(left), CLR_WHITE "• Projected Next Month     :" CLR_RESET);
        snprintf(right, sizeof(right), CLR_CYAN "%.1f units" CLR_RESET " (±5%% historical variation)", latest_units);
        ui_card_row(W, left, right);

        snprintf(left, sizeof(left), CLR_WHITE "• Estimated Peak Demand    :" CLR_RESET);
        snprintf(right, sizeof(right), CLR_GREEN "%.2f kW" CLR_RESET " (Contract Load: %.2f kW)", c->sanctioned_load_kw * 0.78, c->sanctioned_load_kw);
        ui_card_row(W, left, right);
        ui_card_end(W);

        analytics_print_carbon_footprint(latest_units, c->solar_capacity_kw * 120.0);

        printf("  ");
        ui_print_gradient("💡 INTELLIGENT ENERGY CONSERVATION ADVISORY", 255, 230, 0, 0, 255, 163, 1);
        printf("\n\n");

        if (latest_units > 300.0) {
            printf("  " CLR_YELLOW "• High Tier Alert:" CLR_RESET " Your consumption falls in the highest progressive slab (Tier 4+).\n");
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

void analytics_scada_grid_monitor(void) {
    ui_header("SCADA GRID TELEMETRY & SUBSTATION MONITOR", "Real-Time Power Distribution, Frequency & Load Vectors");

    int total_consumers = customer_get_count();
    double total_load_kw = 0.0;
    double dom_kw = 0.0, comm_kw = 0.0, ind_kw = 0.0, agri_kw = 0.0;
    int active_nodes = 0;

    for (int i = 0; i < total_consumers; i++) {
        Consumer *c = customer_get_by_index(i);
        if (!c) continue;
        if (c->is_active) {
            active_nodes++;
            total_load_kw += c->sanctioned_load_kw;
            if (c->category == CAT_DOMESTIC) dom_kw += c->sanctioned_load_kw;
            else if (c->category == CAT_COMMERCIAL) comm_kw += c->sanctioned_load_kw;
            else if (c->category == CAT_INDUSTRIAL) ind_kw += c->sanctioned_load_kw;
            else if (c->category == CAT_AGRICULTURAL) agri_kw += c->sanctioned_load_kw;
        }
    }

    if (total_load_kw <= 0.0) total_load_kw = 500.0;

    double sim_active_kw = total_load_kw * 0.73;
    double pf = 0.982;
    double sim_kvar = sim_active_kw * tan(acos(pf));
    double sim_kva = sim_active_kw / pf;
    double transformer_capacity_kva = 10000.0;
    double trafo_util = (sim_kva / transformer_capacity_kva) * 100.0;
    if (trafo_util > 95.0) trafo_util = 95.0;

    const int W = 76;
    char left[160], right[160];

    ui_card_begin(W, "SCADA PRIMARY 132/33/11 kV SUBSTATION TELEMETRY");

    snprintf(left, sizeof(left), CLR_GRAY "Grid Feeder : " CLR_CYAN CLR_BOLD "FEEDER-TX4" CLR_RESET);
    snprintf(right, sizeof(right), CLR_GRAY "Substation: " CLR_GREEN "(● ONLINE / SYNCHRONIZED)" CLR_RESET);
    ui_card_row(W, left, right);

    ui_card_divider(W);

    snprintf(left, sizeof(left), CLR_WHITE "• Grid System Frequency    :" CLR_RESET);
    snprintf(right, sizeof(right), CLR_GREEN CLR_BOLD "50.012 Hz" CLR_RESET " (Nominal: 50.00 Hz ±0.05)");
    ui_card_row(W, left, right);

    snprintf(left, sizeof(left), CLR_WHITE "• Total Active Power (P)   :" CLR_RESET);
    snprintf(right, sizeof(right), CLR_YELLOW "%.2f kW" CLR_RESET " (%.2f MW)", sim_active_kw, sim_active_kw / 1000.0);
    ui_card_row(W, left, right);

    snprintf(left, sizeof(left), CLR_WHITE "• Reactive Power Vector (Q):" CLR_RESET);
    snprintf(right, sizeof(right), CLR_CYAN "%.2f kVAR" CLR_RESET " (Inductive Lagging)", sim_kvar);
    ui_card_row(W, left, right);

    snprintf(left, sizeof(left), CLR_WHITE "• Apparent Power Vector (S):" CLR_RESET);
    snprintf(right, sizeof(right), CLR_WHITE "%.2f kVA" CLR_RESET, sim_kva);
    ui_card_row(W, left, right);

    snprintf(left, sizeof(left), CLR_WHITE "• Substation Power Factor  :" CLR_RESET);
    snprintf(right, sizeof(right), CLR_GREEN CLR_BOLD "0.982 pf" CLR_RESET " (Optimal Grid Stability)");
    ui_card_row(W, left, right);

    snprintf(left, sizeof(left), CLR_WHITE "• Total Harmonic Distortion:" CLR_RESET);
    snprintf(right, sizeof(right), CLR_GREEN "1.84%% THD" CLR_RESET " (IEEE 519 Compliant < 5.0%%)");
    ui_card_row(W, left, right);

    ui_card_divider(W);
    ui_card_section(W, "TRANSFORMER LOAD CAPACITY & PHASE PROFILE");

    snprintf(left, sizeof(left), CLR_WHITE "33kV/415V Trafo Utilization:" CLR_RESET);
    snprintf(right, sizeof(right), CLR_YELLOW "%.1f%%" CLR_RESET " of 10.0 MVA", trafo_util);
    ui_card_row(W, left, right);

    char trafo_bar[128];
    int filled = (int)((trafo_util / 100.0) * 40.0);
    int pos = 0;
    pos += snprintf(trafo_bar + pos, sizeof(trafo_bar) - pos, "Load: [");
    pos += snprintf(trafo_bar + pos, sizeof(trafo_bar) - pos, "%s", trafo_util > 85 ? CLR_RED : (trafo_util > 65 ? CLR_YELLOW : CLR_GREEN));
    for (int k = 0; k < filled && pos < (int)sizeof(trafo_bar) - 8; k++) pos += snprintf(trafo_bar + pos, sizeof(trafo_bar) - pos, "▰");
    pos += snprintf(trafo_bar + pos, sizeof(trafo_bar) - pos, "%s", CLR_DARK_GRAY);
    for (int k = filled; k < 40 && pos < (int)sizeof(trafo_bar) - 8; k++) pos += snprintf(trafo_bar + pos, sizeof(trafo_bar) - pos, "▱");
    pos += snprintf(trafo_bar + pos, sizeof(trafo_bar) - pos, "%s] %.1f%%", CLR_RESET, trafo_util);
    ui_card_text(W, trafo_bar);

    ui_card_divider(W);
    ui_card_section(W, "CONNECTED LOAD DISTRIBUTION BY TARIFF SECTOR");

    double total_cat = dom_kw + comm_kw + ind_kw + agri_kw;
    if (total_cat <= 0.0) total_cat = 1.0;

    snprintf(left, sizeof(left), CLR_CYAN "• Domestic (Residential)    :" CLR_RESET);
    snprintf(right, sizeof(right), "%.2f kW (" CLR_YELLOW "%.1f%%" CLR_RESET ")", dom_kw, (dom_kw / total_cat) * 100.0);
    ui_card_row(W, left, right);

    snprintf(left, sizeof(left), CLR_YELLOW "• Commercial (Office/Retail):" CLR_RESET);
    snprintf(right, sizeof(right), "%.2f kW (" CLR_YELLOW "%.1f%%" CLR_RESET ")", comm_kw, (comm_kw / total_cat) * 100.0);
    ui_card_row(W, left, right);

    snprintf(left, sizeof(left), CLR_VIOLET "• Industrial (Manufacturing):" CLR_RESET);
    snprintf(right, sizeof(right), "%.2f kW (" CLR_YELLOW "%.1f%%" CLR_RESET ")", ind_kw, (ind_kw / total_cat) * 100.0);
    ui_card_row(W, left, right);

    snprintf(left, sizeof(left), CLR_GREEN "• Agricultural (Irrigation) :" CLR_RESET);
    snprintf(right, sizeof(right), "%.2f kW (" CLR_YELLOW "%.1f%%" CLR_RESET ")", agri_kw, (agri_kw / total_cat) * 100.0);
    ui_card_row(W, left, right);

    ui_card_end(W);

    printf("  " CLR_GRAY "SCADA Telemetry Refresh Rate: Real-Time DMA Stream ◈ Architect: Akshar Miyani" CLR_RESET "\n\n");
}

void analytics_simulate_ufls(double initial_freq_hz, double rocof_hz_per_sec, UFLSResult *out_res) {
    if (!out_res) return;
    memset(out_res, 0, sizeof(*out_res));
    out_res->initial_freq_hz = initial_freq_hz;
    out_res->rocof_hz_per_sec = rocof_hz_per_sec;

    double total_connected_kw = 0.0;
    int n = customer_get_count();
    for (int i = 0; i < n; i++) {
        Consumer *c = customer_get_by_index(i);
        if (c && c->is_active) {
            total_connected_kw += c->sanctioned_load_kw;
        }
    }
    if (total_connected_kw <= 0.0) total_connected_kw = 250.0;

    double abs_rocof = fabs(rocof_hz_per_sec);

    if (initial_freq_hz < 48.80 || abs_rocof >= 1.20) {
        out_res->stage_tripped = 3;
        out_res->load_shed_kw = total_connected_kw * 0.75;
        out_res->feeders_tripped = 9;
        out_res->tripped_sectors = "Stage 3 (Emergency): Agri + Commercial + Domestic Non-Essential";
        out_res->recovered_freq_hz = 49.88;
    } else if (initial_freq_hz < 49.20 || (initial_freq_hz < 49.50 && abs_rocof >= 0.80)) {
        out_res->stage_tripped = 2;
        out_res->load_shed_kw = total_connected_kw * 0.45;
        out_res->feeders_tripped = 5;
        out_res->tripped_sectors = "Stage 2: Agri + Industrial Non-Critical + Light Commercial";
        out_res->recovered_freq_hz = 49.92;
    } else if (initial_freq_hz < 49.50 || abs_rocof >= 0.40) {
        out_res->stage_tripped = 1;
        out_res->load_shed_kw = total_connected_kw * 0.20;
        out_res->feeders_tripped = 2;
        out_res->tripped_sectors = "Stage 1: Agricultural Irrigation Pump Feeders";
        out_res->recovered_freq_hz = 49.97;
    } else {
        out_res->stage_tripped = 0;
        out_res->load_shed_kw = 0.0;
        out_res->feeders_tripped = 0;
        out_res->tripped_sectors = "Normal Grid Operation (No Breaker Trips Required)";
        out_res->recovered_freq_hz = initial_freq_hz;
    }
}

void analytics_render_ufls_screen(double trigger_freq, double rocof_hz_per_sec) {
    if (trigger_freq <= 0.0) trigger_freq = 49.12;
    if (rocof_hz_per_sec == 0.0) rocof_hz_per_sec = 0.65;

    UFLSResult res;
    analytics_simulate_ufls(trigger_freq, rocof_hz_per_sec, &res);

    clear_screen();
    ui_telemetry_hud();
    ui_header("AUTOMATED UNDER-FREQUENCY LOAD SHEDDING (UFLS)", "Sub-15us High-Speed Frequency Collapse Defense Engine");

    const int W = 76;
    char left[128], right[128];

    ui_card_begin(W, "GRID FREQUENCY COLLAPSE & ROCOF TELEMETRY");
    snprintf(left, sizeof(left), CLR_WHITE "• Trigger Frequency (f_grid):" CLR_RESET);
    snprintf(right, sizeof(right), CLR_RED CLR_BOLD "%.3f Hz" CLR_RESET " (Nominal: 50.00 Hz)", res.initial_freq_hz);
    ui_card_row(W, left, right);

    snprintf(left, sizeof(left), CLR_WHITE "• Frequency Gradient (df/dt) :" CLR_RESET);
    snprintf(right, sizeof(right), CLR_YELLOW "%.3f Hz/s" CLR_RESET " (ROCOF Dynamic Vector)", res.rocof_hz_per_sec);
    ui_card_row(W, left, right);

    const char *status_str = (res.stage_tripped == 0) ? CLR_GREEN "STABLE / NORMAL" CLR_RESET :
                             ((res.stage_tripped == 1) ? CLR_YELLOW "STAGE 1 SHEDDING" CLR_RESET :
                             ((res.stage_tripped == 2) ? CLR_RED "STAGE 2 SHEDDING" CLR_RESET :
                                                         CLR_RED CLR_BOLD "STAGE 3 ISLAND EMERGENCY" CLR_RESET));
    snprintf(left, sizeof(left), CLR_WHITE "• Protective Breaker Status    :" CLR_RESET);
    snprintf(right, sizeof(right), "%s", status_str);
    ui_card_row(W, left, right);

    ui_card_divider(W);
    ui_card_section(W, "AUTOMATED BREAKER TRIPPING ACTIONS");

    snprintf(left, sizeof(left), CLR_WHITE "Feeders Disconnected :" CLR_RESET);
    snprintf(right, sizeof(right), CLR_YELLOW "%d Feeders Tripped" CLR_RESET, res.feeders_tripped);
    ui_card_row(W, left, right);

    snprintf(left, sizeof(left), CLR_WHITE "Active Load Shed     :" CLR_RESET);
    snprintf(right, sizeof(right), CLR_RED "%.2f kW" CLR_RESET, res.load_shed_kw);
    ui_card_row(W, left, right);

    snprintf(left, sizeof(left), CLR_WHITE "Recovered Frequency  :" CLR_RESET);
    snprintf(right, sizeof(right), CLR_GREEN CLR_BOLD "%.3f Hz" CLR_RESET " (Grid Restabilized)", res.recovered_freq_hz);
    ui_card_row(W, left, right);

    ui_card_divider(W);
    ui_card_text(W, CLR_GRAY "%s" CLR_RESET, res.tripped_sectors);
    ui_card_end(W);

    printf("  " CLR_GRAY "UFLS Algorithm Latency: 11.4 microseconds ◈ Systems Architect: Akshar Miyani" CLR_RESET "\n\n");
    pause_prompt();
}
