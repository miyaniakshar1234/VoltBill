/**
 * @file tariff.c
 * @brief Dynamic tariff configuration, serialization, and runtime editor.
 * @author Akshar Miyani
 */

#include "tariff.h"
#include "ui.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static TariffConfig g_tariffs[4];
static const char *TARIFF_FILE_PATH = "config/tariffs.cfg";

const char *category_to_string(ConnectionCategory cat) {
    switch (cat) {
        case CAT_DOMESTIC:     return "Domestic (Residential)";
        case CAT_COMMERCIAL:   return "Commercial (Business)";
        case CAT_INDUSTRIAL:   return "Industrial (Heavy)";
        case CAT_AGRICULTURAL: return "Agricultural (Farming)";
        default:               return "Unknown Category";
    }
}

void tariff_reset_defaults(void) {
    /* 1. DOMESTIC TARIFF */
    g_tariffs[0].category = CAT_DOMESTIC;
    strncpy(g_tariffs[0].category_name, "Domestic", 31);
    g_tariffs[0].slab_count = 5;
    g_tariffs[0].slabs[0] = (TariffSlab){0.0, 50.0, 3.15};
    g_tariffs[0].slabs[1] = (TariffSlab){51.0, 150.0, 4.50};
    g_tariffs[0].slabs[2] = (TariffSlab){151.0, 300.0, 6.20};
    g_tariffs[0].slabs[3] = (TariffSlab){301.0, 500.0, 7.10};
    g_tariffs[0].slabs[4] = (TariffSlab){501.0, 999999.0, 7.95};
    g_tariffs[0].fixed_charge_per_kw = 50.00;
    g_tariffs[0].meter_rent = 25.00;
    g_tariffs[0].regulatory_surcharge_pct = 3.8;
    g_tariffs[0].electricity_duty_pct = 5.0;
    g_tariffs[0].green_cess_per_unit = 0.10;
    g_tariffs[0].fppca_per_unit = 0.35;
    g_tariffs[0].prompt_rebate_pct = 1.5;
    g_tariffs[0].late_penalty_pct = 2.0;
    g_tariffs[0].tod_peak_surcharge_pct = 0.0;
    g_tariffs[0].tod_offpeak_rebate_pct = 0.0;

    /* 2. COMMERCIAL TARIFF */
    g_tariffs[1].category = CAT_COMMERCIAL;
    strncpy(g_tariffs[1].category_name, "Commercial", 31);
    g_tariffs[1].slab_count = 4;
    g_tariffs[1].slabs[0] = (TariffSlab){0.0, 100.0, 7.50};
    g_tariffs[1].slabs[1] = (TariffSlab){101.0, 300.0, 8.60};
    g_tariffs[1].slabs[2] = (TariffSlab){301.0, 500.0, 9.40};
    g_tariffs[1].slabs[3] = (TariffSlab){501.0, 999999.0, 10.25};
    g_tariffs[1].fixed_charge_per_kw = 120.00;
    g_tariffs[1].meter_rent = 50.00;
    g_tariffs[1].regulatory_surcharge_pct = 4.5;
    g_tariffs[1].electricity_duty_pct = 8.0;
    g_tariffs[1].green_cess_per_unit = 0.15;
    g_tariffs[1].fppca_per_unit = 0.45;
    g_tariffs[1].prompt_rebate_pct = 1.0;
    g_tariffs[1].late_penalty_pct = 2.5;
    g_tariffs[1].tod_peak_surcharge_pct = 15.0;
    g_tariffs[1].tod_offpeak_rebate_pct = 5.0;

    /* 3. INDUSTRIAL TARIFF */
    g_tariffs[2].category = CAT_INDUSTRIAL;
    strncpy(g_tariffs[2].category_name, "Industrial", 31);
    g_tariffs[2].slab_count = 3;
    g_tariffs[2].slabs[0] = (TariffSlab){0.0, 500.0, 8.20};
    g_tariffs[2].slabs[1] = (TariffSlab){501.0, 2000.0, 7.80};
    g_tariffs[2].slabs[2] = (TariffSlab){2001.0, 999999.0, 7.40};
    g_tariffs[2].fixed_charge_per_kw = 200.00;
    g_tariffs[2].meter_rent = 150.00;
    g_tariffs[2].regulatory_surcharge_pct = 5.0;
    g_tariffs[2].electricity_duty_pct = 9.0;
    g_tariffs[2].green_cess_per_unit = 0.20;
    g_tariffs[2].fppca_per_unit = 0.55;
    g_tariffs[2].prompt_rebate_pct = 2.0;
    g_tariffs[2].late_penalty_pct = 3.0;
    g_tariffs[2].tod_peak_surcharge_pct = 20.0;
    g_tariffs[2].tod_offpeak_rebate_pct = 10.0;

    /* 4. AGRICULTURAL TARIFF */
    g_tariffs[3].category = CAT_AGRICULTURAL;
    strncpy(g_tariffs[3].category_name, "Agricultural", 31);
    g_tariffs[3].slab_count = 1;
    g_tariffs[3].slabs[0] = (TariffSlab){0.0, 999999.0, 1.25};
    g_tariffs[3].fixed_charge_per_kw = 15.00;
    g_tariffs[3].meter_rent = 10.00;
    g_tariffs[3].regulatory_surcharge_pct = 1.0;
    g_tariffs[3].electricity_duty_pct = 1.0;
    g_tariffs[3].green_cess_per_unit = 0.00;
    g_tariffs[3].fppca_per_unit = 0.10;
    g_tariffs[3].prompt_rebate_pct = 2.0;
    g_tariffs[3].late_penalty_pct = 1.0;
    g_tariffs[3].tod_peak_surcharge_pct = 0.0;
    g_tariffs[3].tod_offpeak_rebate_pct = 0.0;
}

int tariff_save_to_file(void) {
    ensure_directory("config");
    FILE *fp = fopen(TARIFF_FILE_PATH, "w");
    if (!fp) return 0;

    fprintf(fp, "# VoltBill Tariff Configuration\n");
    fprintf(fp, "# Maintained by Akshar Miyani\n\n");

    for (int i = 0; i < 4; i++) {
        TariffConfig *t = &g_tariffs[i];
        fprintf(fp, "[CATEGORY_%d]\n", t->category);
        fprintf(fp, "name=%s\n", t->category_name);
        fprintf(fp, "slab_count=%d\n", t->slab_count);
        for (int s = 0; s < t->slab_count; s++) {
            fprintf(fp, "slab_%d=%.2f,%.2f,%.2f\n", s, 
                    t->slabs[s].min_units, t->slabs[s].max_units, t->slabs[s].rate_per_unit);
        }
        fprintf(fp, "fixed_charge=%.2f\n", t->fixed_charge_per_kw);
        fprintf(fp, "meter_rent=%.2f\n", t->meter_rent);
        fprintf(fp, "regulatory_pct=%.2f\n", t->regulatory_surcharge_pct);
        fprintf(fp, "duty_pct=%.2f\n", t->electricity_duty_pct);
        fprintf(fp, "green_cess=%.2f\n", t->green_cess_per_unit);
        fprintf(fp, "fppca=%.2f\n", t->fppca_per_unit);
        fprintf(fp, "prompt_rebate=%.2f\n", t->prompt_rebate_pct);
        fprintf(fp, "late_penalty=%.2f\n", t->late_penalty_pct);
        fprintf(fp, "tod_peak_surcharge=%.2f\n", t->tod_peak_surcharge_pct);
        fprintf(fp, "tod_offpeak_rebate=%.2f\n\n", t->tod_offpeak_rebate_pct);
    }
    fclose(fp);
    return 1;
}

int tariff_load_from_file(void) {
    FILE *fp = fopen(TARIFF_FILE_PATH, "r");
    if (!fp) {
        tariff_reset_defaults();
        tariff_save_to_file();
        return 1;
    }

    char line[256];
    int current_cat = -1;

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        if (sscanf(line, "[CATEGORY_%d]", &current_cat) == 1) {
            if (current_cat >= 0 && current_cat < 4) {
                g_tariffs[current_cat].category = (ConnectionCategory)current_cat;
            }
            continue;
        }

        if (current_cat < 0 || current_cat >= 4) continue;
        TariffConfig *t = &g_tariffs[current_cat];

        char key[64], val[128];
        if (sscanf(line, "%63[^=]=%127[^\r\n]", key, val) == 2) {
            if (strcmp(key, "name") == 0) {
                strncpy(t->category_name, val, 31);
            } else if (strcmp(key, "slab_count") == 0) {
                t->slab_count = atoi(val);
                if (t->slab_count > MAX_SLABS) t->slab_count = MAX_SLABS;
            } else if (strncmp(key, "slab_", 5) == 0) {
                int s_idx = atoi(key + 5);
                if (s_idx >= 0 && s_idx < MAX_SLABS) {
                    double min_u, max_u, rate;
                    if (sscanf(val, "%lf,%lf,%lf", &min_u, &max_u, &rate) == 3) {
                        t->slabs[s_idx] = (TariffSlab){min_u, max_u, rate};
                    }
                }
            } else if (strcmp(key, "fixed_charge") == 0) {
                t->fixed_charge_per_kw = atof(val);
            } else if (strcmp(key, "meter_rent") == 0) {
                t->meter_rent = atof(val);
            } else if (strcmp(key, "regulatory_pct") == 0) {
                t->regulatory_surcharge_pct = atof(val);
            } else if (strcmp(key, "duty_pct") == 0) {
                t->electricity_duty_pct = atof(val);
            } else if (strcmp(key, "green_cess") == 0) {
                t->green_cess_per_unit = atof(val);
            } else if (strcmp(key, "fppca") == 0) {
                t->fppca_per_unit = atof(val);
            } else if (strcmp(key, "prompt_rebate") == 0) {
                t->prompt_rebate_pct = atof(val);
            } else if (strcmp(key, "late_penalty") == 0) {
                t->late_penalty_pct = atof(val);
            } else if (strcmp(key, "tod_peak_surcharge") == 0) {
                t->tod_peak_surcharge_pct = atof(val);
            } else if (strcmp(key, "tod_offpeak_rebate") == 0) {
                t->tod_offpeak_rebate_pct = atof(val);
            }
        }
    }
    fclose(fp);
    return 1;
}

void tariff_init(void) {
    tariff_load_from_file();
}

TariffConfig *tariff_get(ConnectionCategory cat) {
    if (cat >= 0 && cat < 4) {
        return &g_tariffs[cat];
    }
    return &g_tariffs[0];
}

void tariff_display_all(void) {
    ui_header("TARIFF CONFIGURATION & SLAB RULES", "Active Electricity Slabs & Government Surcharges");

    for (int i = 0; i < 4; i++) {
        TariffConfig *t = &g_tariffs[i];
        printf("  " CLR_CYAN CLR_BOLD "● %s Tariffs" CLR_RESET "\n", category_to_string(t->category));
        printf("  " CLR_GRAY "┌───────────────────────────────────────────────┬────────────────────┐" CLR_RESET "\n");
        printf("  " CLR_GRAY "│ " CLR_WHITE CLR_BOLD "%-45s" CLR_RESET CLR_GRAY "│ " CLR_YELLOW CLR_BOLD "%-18s" CLR_RESET CLR_GRAY "│" CLR_RESET "\n", "Consumption Slab Range", "Rate (₹/kWh)");
        printf("  " CLR_GRAY "├───────────────────────────────────────────────┼────────────────────┤" CLR_RESET "\n");

        for (int s = 0; s < t->slab_count; s++) {
            char range_str[48];
            if (t->slabs[s].max_units >= 999990.0) {
                snprintf(range_str, sizeof(range_str), "Above %.0f units", t->slabs[s].min_units);
            } else {
                snprintf(range_str, sizeof(range_str), "%.0f to %.0f units", t->slabs[s].min_units, t->slabs[s].max_units);
            }
            printf("  " CLR_GRAY "│ " CLR_WHITE "%-45s" CLR_RESET CLR_GRAY "│ " CLR_YELLOW "₹ %-16.2f" CLR_RESET CLR_GRAY "│" CLR_RESET "\n", 
                   range_str, t->slabs[s].rate_per_unit);
        }

        printf("  " CLR_GRAY "├───────────────────────────────────────────────┴────────────────────┤" CLR_RESET "\n");
        printf("  " CLR_GRAY "│ " CLR_WHITE "Fixed Charge: ₹%.2f/kW/mo  •  Meter Rent: ₹%.2f/mo  •  Duty: %.1f%%" CLR_RESET "\n",
               t->fixed_charge_per_kw, t->meter_rent, t->electricity_duty_pct);
        printf("  " CLR_GRAY "│ " CLR_WHITE "Regulatory Surcharge: %.1f%%  •  Fuel Adj: ₹%.2f/u  •  Rebate: %.1f%%" CLR_RESET "\n",
               t->regulatory_surcharge_pct, t->fppca_per_unit, t->prompt_rebate_pct);
        if (t->tod_peak_surcharge_pct > 0.0) {
            printf("  " CLR_GRAY "│ " CLR_YELLOW "Time-of-Day (ToD): Peak Surcharge: +%.1f%%  •  Off-Peak Rebate: -%.1f%%" CLR_RESET "\n",
                   t->tod_peak_surcharge_pct, t->tod_offpeak_rebate_pct);
        }
        printf("  " CLR_GRAY "└────────────────────────────────────────────────────────────────────┘" CLR_RESET "\n\n");
    }

    pause_prompt();
}

void tariff_edit_menu(void) {
    const char *cat_opts[] = {
        "Domestic (Residential)",
        "Commercial (Offices / Shops)",
        "Industrial (Manufacturing)",
        "Agricultural (Farming)",
        "Reset All to Standard System Defaults"
    };

    int sel = ui_menu("EDIT TARIFF CONFIGURATION", cat_opts, 5, 0);
    if (sel < 0) return;

    if (sel == 4) {
        tariff_reset_defaults();
        tariff_save_to_file();
        audit_log("TARIFF_RESET", "Restored all tariffs to system defaults");
        ui_message_box("Tariffs Reset", "All tariffs restored to standard defaults successfully!", 1);
        return;
    }

    TariffConfig *t = &g_tariffs[sel];
    ui_header("MODIFY TARIFF RATES", t->category_name);

    printf("  " CLR_WHITE "Editing rates for: " CLR_CYAN CLR_BOLD "%s" CLR_RESET "\n\n", category_to_string(t->category));
    
    for (int s = 0; s < t->slab_count; s++) {
        char prompt[128];
        snprintf(prompt, sizeof(prompt), "  Enter new rate for Slab %d [%.0f - %.0f] (Current: ₹%.2f): ₹ ",
                 s + 1, t->slabs[s].min_units, t->slabs[s].max_units, t->slabs[s].rate_per_unit);
        t->slabs[s].rate_per_unit = get_safe_double(prompt, 0.10, 100.0);
    }

    t->fixed_charge_per_kw = get_safe_double("  Fixed Charge per kW (₹/mo): ₹ ", 0.0, 1000.0);
    t->meter_rent = get_safe_double("  Meter Rent per month: ₹ ", 0.0, 500.0);
    t->electricity_duty_pct = get_safe_double("  Electricity Duty Tax (%): ", 0.0, 50.0);

    tariff_save_to_file();
    audit_log("TARIFF_UPDATE", t->category_name);
    ui_message_box("Tariff Saved", "Updated rates saved to config/tariffs.cfg successfully!", 1);
}
