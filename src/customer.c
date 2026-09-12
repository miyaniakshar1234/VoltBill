/**
 * @file customer.c
 * @brief Implementation of customer registration, search, and profile rendering.
 * @author Akshar Miyani
 */

#include "customer.h"
#include "tariff.h"
#include "ui.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static Consumer g_consumers[MAX_CONSUMERS];
static int g_consumer_count = 0;

void customer_init(void) {
    g_consumer_count = 0;
}

int customer_get_count(void) {
    return g_consumer_count;
}

Consumer *customer_get_by_index(int index) {
    if (index >= 0 && index < g_consumer_count) {
        return &g_consumers[index];
    }
    return NULL;
}

Consumer *customer_find_by_id(const char *id) {
    if (!id || id[0] == '\0') return NULL;
    for (int i = 0; i < g_consumer_count; i++) {
        if (strcmp(g_consumers[i].id, id) == 0) {
            return &g_consumers[i];
        }
    }
    return NULL;
}

Consumer *customer_find_by_meter(const char *meter_no) {
    if (!meter_no || meter_no[0] == '\0') return NULL;
    for (int i = 0; i < g_consumer_count; i++) {
        if (strcmp(g_consumers[i].meter_no, meter_no) == 0) {
            return &g_consumers[i];
        }
    }
    return NULL;
}

int customer_add_record(const Consumer *c) {
    if (g_consumer_count >= MAX_CONSUMERS || !c) return 0;
    g_consumers[g_consumer_count++] = *c;
    return 1;
}

void customer_clear_all(void) {
    g_consumer_count = 0;
}

static void generate_next_consumer_id(char *dest, size_t max_len) {
    int max_num = 1000;
    for (int i = 0; i < g_consumer_count; i++) {
        int num = 0;
        if (sscanf(g_consumers[i].id, "VB-%d", &num) == 1) {
            if (num > max_num) max_num = num;
        }
    }
    snprintf(dest, max_len, "VB-%04d", max_num + 1);
}

void customer_render_card(const Consumer *c) {
    if (!c) return;

    const int W = 76;
    char left[160], right[160], full[256];

    ui_card_begin(W, "CONSUMER PROFILE & GRID NODE RECORD");

    /* Header Row */
    snprintf(left, sizeof(left), CLR_CYAN CLR_BOLD "%s" CLR_RESET "  " CLR_WHITE CLR_BOLD "%-36.36s" CLR_RESET, c->id, c->name);
    if (c->is_active) {
        if (c->is_flagged_for_disconnection) {
            snprintf(right, sizeof(right), CLR_YELLOW "(▲ NOTICE SERVED)" CLR_RESET);
        } else {
            snprintf(right, sizeof(right), CLR_GREEN "(● ACTIVE NODE)" CLR_RESET);
        }
    } else {
        snprintf(right, sizeof(right), CLR_RED "(■ DISCONNECTED)" CLR_RESET);
    }
    ui_card_row(W, left, right);

    ui_card_divider(W);

    snprintf(left, sizeof(left), CLR_GRAY "Tariff Class : " CLR_WHITE "%-20s" CLR_RESET, category_to_string(c->category));
    snprintf(right, sizeof(right), CLR_GRAY "Meter Serial : " CLR_WHITE "%-20s" CLR_RESET, c->meter_no);
    ui_card_row(W, left, right);

    snprintf(left, sizeof(left), CLR_GRAY "Phase Supply : " CLR_WHITE "%-20s" CLR_RESET, c->phase == PHASE_THREE ? "3-Phase (415V)" : "1-Phase (230V)");
    snprintf(right, sizeof(right), CLR_GRAY "Contract Load: " CLR_YELLOW "%.2f kW" CLR_RESET, c->sanctioned_load_kw);
    ui_card_row(W, left, right);

    snprintf(left, sizeof(left), CLR_GRAY "Phone Contact: " CLR_WHITE "%-20s" CLR_RESET, c->phone);
    snprintf(right, sizeof(right), CLR_GRAY "Solar Rooftop: " CLR_GREEN "%.2f kW" CLR_RESET, c->solar_capacity_kw);
    ui_card_row(W, left, right);

    snprintf(left, sizeof(left), CLR_GRAY "Email Address: " CLR_WHITE "%-20s" CLR_RESET, c->email);
    snprintf(right, sizeof(right), CLR_GRAY "Registration : " CLR_WHITE "%-20s" CLR_RESET, c->registered_date);
    ui_card_row(W, left, right);

    snprintf(full, sizeof(full), CLR_GRAY "Address      : " CLR_WHITE "%-58.58s" CLR_RESET, c->address);
    ui_card_text(W, full);

    ui_card_divider(W);

    /* Demand Load Gauge */
    char gauge_str[128];
    double ratio = c->sanctioned_load_kw / 50.0;
    if (ratio > 1.0) ratio = 1.0;
    int filled = (int)(ratio * 20.0);
    int pos = 0;
    pos += snprintf(gauge_str + pos, sizeof(gauge_str) - pos, "[");
    pos += snprintf(gauge_str + pos, sizeof(gauge_str) - pos, "%s", ratio > 0.85 ? CLR_RED : (ratio > 0.6 ? CLR_YELLOW : CLR_GREEN));
    for (int i = 0; i < filled && pos < (int)sizeof(gauge_str) - 8; i++) {
        pos += snprintf(gauge_str + pos, sizeof(gauge_str) - pos, "▰");
    }
    pos += snprintf(gauge_str + pos, sizeof(gauge_str) - pos, "%s", CLR_DARK_GRAY);
    for (int i = filled; i < 20 && pos < (int)sizeof(gauge_str) - 8; i++) {
        pos += snprintf(gauge_str + pos, sizeof(gauge_str) - pos, "▱");
    }
    pos += snprintf(gauge_str + pos, sizeof(gauge_str) - pos, "%s] %5.1f%% (%.1f / 50.0 kW)", CLR_RESET, ratio * 100.0, c->sanctioned_load_kw);

    snprintf(left, sizeof(left), CLR_GRAY "Contract Demand Load Gauge:" CLR_RESET);
    ui_card_row(W, left, gauge_str);

    ui_card_divider(W);

    char bal_str[32], cred_str[32];
    format_currency(c->outstanding_arrears, bal_str, sizeof(bal_str));
    format_currency(c->advance_credit, cred_str, sizeof(cred_str));

    snprintf(left, sizeof(left), CLR_GRAY "Unpaid Arrears : " CLR_RED CLR_BOLD "%-16s" CLR_RESET, bal_str);
    snprintf(right, sizeof(right), CLR_GRAY "Advance Credit : " CLR_GREEN CLR_BOLD "%-16s" CLR_RESET, cred_str);
    ui_card_row(W, left, right);

    ui_card_end(W);
    printf("\n");
}

void customer_quick_status(const char *consumer_id) {
    Consumer *c = customer_find_by_id(consumer_id);
    if (!c) {
        printf("VoltBill: Consumer '%s' not found.\n", consumer_id);
        return;
    }
    printf("Consumer: %s (%s) | Meter: %s | Load: %.2f kW | Category: %s | Arrears: Rs. %.2f | Status: %s\n",
           c->id, c->name, c->meter_no, c->sanctioned_load_kw, category_to_string(c->category),
           c->outstanding_arrears, c->is_active ? "Active" : "Disconnected");
}

void customer_register_flow(void) {
    ui_header("CUSTOMER ONBOARDING & REGISTRATION", "Add New Electricity Connection to Utility Grid");

    if (g_consumer_count >= MAX_CONSUMERS) {
        ui_message_box("Registry Full", "Maximum consumer capacity reached.", 0);
        return;
    }

    Consumer c;
    memset(&c, 0, sizeof(c));

    generate_next_consumer_id(c.id, sizeof(c.id));
    printf("  " CLR_GRAY "Generated Consumer ID :" CLR_RESET " " CLR_CYAN CLR_BOLD "%s" CLR_RESET "\n\n", c.id);

    printf("  " CLR_WHITE "Enter Full Name       : " CLR_RESET);
    get_safe_string(c.name, sizeof(c.name));
    if (c.name[0] == '\0') {
        ui_message_box("Registration Cancelled", "Name cannot be empty.", 0);
        return;
    }

    printf("  " CLR_WHITE "Enter Mobile Number   : " CLR_RESET);
    get_safe_string(c.phone, sizeof(c.phone));

    printf("  " CLR_WHITE "Enter Email Address   : " CLR_RESET);
    get_safe_string(c.email, sizeof(c.email));

    printf("  " CLR_WHITE "Enter Physical Address: " CLR_RESET);
    get_safe_string(c.address, sizeof(c.address));

    /* Meter serial validation */
    while (1) {
        printf("  " CLR_WHITE "Enter Meter Serial No : " CLR_RESET);
        get_safe_string(c.meter_no, sizeof(c.meter_no));
        if (c.meter_no[0] == '\0') {
            printf("  \033[91m[Error] Meter number cannot be empty.\033[0m\n");
            continue;
        }
        if (customer_find_by_meter(c.meter_no) != NULL) {
            printf("  \033[91m[Error] Meter '%s' is already registered to another consumer!\033[0m\n", c.meter_no);
            continue;
        }
        break;
    }

    /* Connection Category */
    printf("\n  " CLR_CYAN "Connection Categories:" CLR_RESET "\n");
    printf("    [0] Domestic (Residential)\n");
    printf("    [1] Commercial (Business / Office)\n");
    printf("    [2] Industrial (Heavy Load)\n");
    printf("    [3] Agricultural (Farming)\n");
    c.category = (ConnectionCategory)get_safe_int("  Select Category [0-3]: ", 0, 3);

    /* Phase Selection */
    int phase_choice = get_safe_int("  Supply Phase [1 for Single Phase, 3 for Three Phase]: ", 1, 3);
    c.phase = (phase_choice == 3) ? PHASE_THREE : PHASE_SINGLE;

    /* Sanctioned Load */
    c.sanctioned_load_kw = get_safe_double("  Sanctioned Contract Load in kW (e.g., 2.0 to 150.0): ", 0.5, 1000.0);

    /* Security Deposit */
    c.security_deposit = get_safe_double("  Initial Security Deposit (₹): ₹ ", 0.0, 1000000.0);

    /* Solar Rooftop Net Metering */
    int has_solar = get_safe_int("  Has Rooftop Solar Installed? [1 = Yes, 0 = No]: ", 0, 1);
    if (has_solar) {
        c.solar_capacity_kw = get_safe_double("  Solar Generation Capacity in kW: ", 0.5, 100.0);
    } else {
        c.solar_capacity_kw = 0.0;
    }

    c.outstanding_arrears = 0.0;
    c.advance_credit = 0.0;
    c.is_active = 1;
    c.is_flagged_for_disconnection = 0;
    get_current_date(c.registered_date, sizeof(c.registered_date));

    customer_add_record(&c);

    extern int storage_save_all(void);
    storage_save_all();

    char audit_desc[128];
    snprintf(audit_desc, sizeof(audit_desc), "Registered consumer %s (%s, %.1f kW)", c.id, c.name, c.sanctioned_load_kw);
    audit_log("CONSUMER_REGISTER", audit_desc);

    char msg[128];
    snprintf(msg, sizeof(msg), "Consumer %s (%s) registered successfully!", c.id, c.name);
    ui_message_box("Registration Complete", msg, 1);
}

void customer_list_all(void) {
    ui_header("REGISTERED CONSUMERS DIRECTORY", "Grid Connection Roster & Current Ledger Status");

    if (g_consumer_count == 0) {
        printf("  " CLR_YELLOW "No consumers registered yet. Use Option 1 or Seed Demo Data to add." CLR_RESET "\n");
        pause_prompt();
        return;
    }

    printf("  " CLR_GRAY "┌──────────┬──────────────────────────┬──────────────┬─────────────┬──────────┬───────────┐" CLR_RESET "\n");
    printf("  " CLR_GRAY "│ " CLR_CYAN CLR_BOLD "%-8s" CLR_RESET CLR_GRAY "│ " 
           CLR_WHITE CLR_BOLD "%-24s" CLR_RESET CLR_GRAY "│ " 
           CLR_YELLOW CLR_BOLD "%-12s" CLR_RESET CLR_GRAY "│ " 
           CLR_WHITE CLR_BOLD "%-11s" CLR_RESET CLR_GRAY "│ " 
           CLR_WHITE CLR_BOLD "%-8s" CLR_RESET CLR_GRAY "│ " 
           CLR_GREEN CLR_BOLD "%-9s" CLR_RESET CLR_GRAY "│" CLR_RESET "\n",
           "ID", "Consumer Name", "Category", "Meter No", "Load(kW)", "Status");
    printf("  " CLR_GRAY "├──────────┼──────────────────────────┼──────────────┼─────────────┼──────────┼───────────┤" CLR_RESET "\n");

    for (int i = 0; i < g_consumer_count; i++) {
        Consumer *c = &g_consumers[i];
        const char *cat_short = "Domestic";
        if (c->category == CAT_COMMERCIAL) cat_short = "Commercial";
        else if (c->category == CAT_INDUSTRIAL) cat_short = "Industrial";
        else if (c->category == CAT_AGRICULTURAL) cat_short = "Agri";

        const char *st_clr = c->is_active ? CLR_GREEN : CLR_RED;
        const char *st_txt = c->is_active ? "Active" : "Disconnected";
        if (c->is_active && c->is_flagged_for_disconnection) {
            st_clr = CLR_YELLOW;
            st_txt = "Notice";
        }

        printf("  " CLR_GRAY "│ " CLR_CYAN "%-8s" CLR_RESET CLR_GRAY "│ " 
               CLR_WHITE "%-24.24s" CLR_RESET CLR_GRAY "│ " 
               CLR_YELLOW "%-12s" CLR_RESET CLR_GRAY "│ " 
               CLR_GRAY "%-11s" CLR_RESET CLR_GRAY "│ " 
               CLR_WHITE "%7.2f " CLR_RESET CLR_GRAY "│ " 
               "%s%-9s" CLR_RESET CLR_GRAY "│" CLR_RESET "\n",
               c->id, c->name, cat_short, c->meter_no, c->sanctioned_load_kw,
               st_clr, st_txt);
    }

    printf("  " CLR_GRAY "└──────────┴──────────────────────────┴──────────────┴─────────────┴──────────┴───────────┘" CLR_RESET "\n");
    printf("  " CLR_GRAY "Total Registered Consumers: " CLR_CYAN CLR_BOLD "%d" CLR_RESET "\n", g_consumer_count);

    pause_prompt();
}

void customer_search_flow(void) {
    ui_header("CUSTOMER LOOKUP & PROFILE SEARCH", "Query Consumers by ID, Name, Phone, or Meter Serial");

    if (g_consumer_count == 0) {
        printf("  " CLR_YELLOW "No consumers in database to search." CLR_RESET "\n");
        pause_prompt();
        return;
    }

    printf("  " CLR_WHITE "Enter Consumer ID, Meter Serial, or Name: " CLR_RESET);
    char query[64];
    get_safe_string(query, sizeof(query));

    if (query[0] == '\0') return;

    Consumer *found = customer_find_by_id(query);
    if (found) {
        printf("\n  " CLR_GREEN "✓ Consumer ID match found:" CLR_RESET "\n\n");
        customer_render_card(found);
        pause_prompt();
        return;
    }

    found = customer_find_by_meter(query);
    if (found) {
        printf("\n  " CLR_GREEN "✓ Meter Serial match found:" CLR_RESET "\n\n");
        customer_render_card(found);
        pause_prompt();
        return;
    }

    int match_count = 0;
    char query_lower[64];
    for (size_t i = 0; i < sizeof(query_lower) && query[i]; i++) {
        query_lower[i] = (char)tolower((unsigned char)query[i]);
        query_lower[i + 1] = '\0';
    }

    printf("\n");
    for (int i = 0; i < g_consumer_count; i++) {
        char name_lower[NAME_LEN];
        for (size_t k = 0; k < sizeof(name_lower) && g_consumers[i].name[k]; k++) {
            name_lower[k] = (char)tolower((unsigned char)g_consumers[i].name[k]);
            name_lower[k + 1] = '\0';
        }

        if (strstr(name_lower, query_lower) != NULL || strstr(g_consumers[i].phone, query) != NULL) {
            customer_render_card(&g_consumers[i]);
            match_count++;
        }
    }

    if (match_count == 0) {
        ui_message_box("Search Result", "No consumers matching query were found.", 0);
    } else {
        printf("  " CLR_GRAY "Total Matches: " CLR_CYAN "%d" CLR_RESET "\n", match_count);
        pause_prompt();
    }
}

void customer_edit_flow(void) {
    ui_header("EDIT CONSUMER RECORD", "Update Contact Info, Sanctioned Load, or Solar Setup");

    printf("  " CLR_WHITE "Enter Consumer ID to modify: " CLR_RESET);
    char id[ID_LEN];
    get_safe_string(id, sizeof(id));

    Consumer *c = customer_find_by_id(id);
    if (!c) {
        ui_message_box("Not Found", "Consumer ID not found in system.", 0);
        return;
    }

    customer_render_card(c);

    printf("  " CLR_CYAN "Select attribute to edit:" CLR_RESET "\n");
    printf("    [1] Mobile Phone Number\n");
    printf("    [2] Email Address\n");
    printf("    [3] Physical Address\n");
    printf("    [4] Sanctioned Contract Load (kW)\n");
    printf("    [5] Solar Rooftop Capacity (kW)\n");
    printf("    [6] Toggle Active / Disconnected Status\n");
    printf("    [0] Cancel\n");

    int choice = get_safe_int("  Select [0-6]: ", 0, 6);
    if (choice == 0) return;

    if (choice == 1) {
        printf("  Current Phone: %s\n  New Phone: ", c->phone);
        get_safe_string(c->phone, sizeof(c->phone));
    } else if (choice == 2) {
        printf("  Current Email: %s\n  New Email: ", c->email);
        get_safe_string(c->email, sizeof(c->email));
    } else if (choice == 3) {
        printf("  Current Address: %s\n  New Address: ", c->address);
        get_safe_string(c->address, sizeof(c->address));
    } else if (choice == 4) {
        c->sanctioned_load_kw = get_safe_double("  Enter New Sanctioned Load in kW: ", 0.5, 1000.0);
    } else if (choice == 5) {
        c->solar_capacity_kw = get_safe_double("  Enter New Solar Capacity in kW: ", 0.0, 100.0);
    } else if (choice == 6) {
        c->is_active = !c->is_active;
        printf("  Status switched to: %s\n", c->is_active ? "ACTIVE" : "DISCONNECTED");
    }

    extern int storage_save_all(void);
    storage_save_all();

    char audit_desc[128];
    snprintf(audit_desc, sizeof(audit_desc), "Modified profile for consumer %s", c->id);
    audit_log("CONSUMER_EDIT", audit_desc);

    ui_message_box("Profile Updated", "Consumer record updated successfully!", 1);
}

void customer_defaulters_flow(void) {
    ui_header("DEFAULTER LEDGER & DISCONNECTION NOTICES", "Flag Consumers with Outstanding Arrears & Issue Legal Notices");

    double threshold = get_safe_double("  Enter Arrears Threshold for Defaulter Screening (₹): ₹ ", 0.0, 1000000.0);

    int defaulter_count = 0;
    Consumer *defaulters[MAX_CONSUMERS];

    for (int i = 0; i < g_consumer_count; i++) {
        if (g_consumers[i].outstanding_arrears >= threshold && g_consumers[i].is_active) {
            defaulters[defaulter_count++] = &g_consumers[i];
        }
    }

    if (defaulter_count == 0) {
        ui_message_box("Zero Defaulters", "No active consumers found exceeding specified arrears threshold.", 1);
        return;
    }

    printf("\n  " CLR_GRAY "┌──────────┬──────────────────────────┬────────────────┬──────────────────┐" CLR_RESET "\n");
    printf("  " CLR_GRAY "│ " CLR_CYAN CLR_BOLD "%-8s" CLR_RESET CLR_GRAY "│ " 
           CLR_WHITE CLR_BOLD "%-24s" CLR_RESET CLR_GRAY "│ " 
           CLR_RED CLR_BOLD "%-14s" CLR_RESET CLR_GRAY "│ " 
           CLR_YELLOW CLR_BOLD "%-16s" CLR_RESET CLR_GRAY "│" CLR_RESET "\n",
           "ID", "Consumer Name", "Arrears (Rs.)", "Notice Status");
    printf("  " CLR_GRAY "├──────────┼──────────────────────────┼────────────────┼──────────────────┤" CLR_RESET "\n");

    for (int i = 0; i < defaulter_count; i++) {
        Consumer *d = defaulters[i];
        char amt_str[32];
        snprintf(amt_str, sizeof(amt_str), "Rs. %10.2f", d->outstanding_arrears);
        printf("  " CLR_GRAY "│ " CLR_CYAN "%-8s" CLR_RESET CLR_GRAY "│ " 
               CLR_WHITE "%-24.24s" CLR_RESET CLR_GRAY "│ " 
               CLR_RED "%-14s" CLR_RESET CLR_GRAY "│ " 
               CLR_YELLOW "%-16s" CLR_RESET CLR_GRAY "│" CLR_RESET "\n",
               d->id, d->name, amt_str,
               d->is_flagged_for_disconnection ? "Notice Served" : "Pending Action");
    }
    printf("  " CLR_GRAY "└──────────┴──────────────────────────┴────────────────┴──────────────────┘" CLR_RESET "\n\n");

    printf("  " CLR_CYAN "Actions:" CLR_RESET "\n");
    printf("    [1] Generate Official Disconnection Notice for a Consumer\n");
    printf("    [2] Disconnect Power Supply for Flagged Defaulter\n");
    printf("    [0] Return\n");

    int action = get_safe_int("  Select Action [0-2]: ", 0, 2);
    if (action == 1) {
        printf("  Enter Consumer ID to serve notice: ");
        char cid[ID_LEN];
        get_safe_string(cid, sizeof(cid));
        Consumer *target = customer_find_by_id(cid);
        if (target) {
            target->is_flagged_for_disconnection = 1;
            ensure_directory("data/notices");
            char notice_path[128];
            snprintf(notice_path, sizeof(notice_path), "data/notices/NOTICE_%s.txt", target->id);

            FILE *nfp = fopen(notice_path, "w");
            if (nfp) {
                char today[DATE_LEN], final_date[DATE_LEN];
                get_current_date(today, sizeof(today));
                compute_due_date(today, 7, final_date, sizeof(final_date));

                fprintf(nfp, "========================================================================\n");
                fprintf(nfp, "                   VOLTBILL UTILITY DISTRIBUTION CORP                   \n");
                fprintf(nfp, "              OFFICIAL FINAL POWER DISCONNECTION NOTICE                \n");
                fprintf(nfp, "========================================================================\n");
                fprintf(nfp, "Notice Date: %s                                     Ref: DISC-%s\n", today, target->id);
                fprintf(nfp, "To:\n");
                fprintf(nfp, "  Consumer ID : %s\n", target->id);
                fprintf(nfp, "  Name        : %s\n", target->name);
                fprintf(nfp, "  Meter No    : %s\n", target->meter_no);
                fprintf(nfp, "  Address     : %s\n\n", target->address);
                fprintf(nfp, "DEMAND STATEMENT:\n");
                fprintf(nfp, "  Our records show that your electricity account has an outstanding\n");
                fprintf(nfp, "  unpaid balance of Rs. %.2f.\n\n", target->outstanding_arrears);
                fprintf(nfp, "FINAL SETTLEMENT DEADLINE: %s\n\n", final_date);
                fprintf(nfp, "Please note that failure to clear the outstanding balance by %s\n", final_date);
                fprintf(nfp, "will result in IMMEDIATE PHYSICAL DISCONNECTION of your power service.\n");
                fprintf(nfp, "Reconnection will attract a statutory restoration surcharge of Rs. 500.00.\n");
                fprintf(nfp, "========================================================================\n");
                fprintf(nfp, "Issued by VoltBill Utility System Engine | Author: Akshar Miyani\n");
                fclose(nfp);

                char audit_desc[128];
                snprintf(audit_desc, sizeof(audit_desc), "Served disconnection notice to %s (Arrears: Rs. %.2f)", target->id, target->outstanding_arrears);
                audit_log("DISCONNECT_NOTICE", audit_desc);

                extern int storage_save_all(void);
                storage_save_all();

                char msg[128];
                snprintf(msg, sizeof(msg), "Notice generated and saved to %s", notice_path);
                ui_message_box("Notice Issued", msg, 1);
            }
        } else {
            ui_message_box("Not Found", "Consumer ID was not found.", 0);
        }
    } else if (action == 2) {
        printf("  Enter Consumer ID to disconnect: ");
        char cid[ID_LEN];
        get_safe_string(cid, sizeof(cid));
        Consumer *target = customer_find_by_id(cid);
        if (target) {
            target->is_active = 0;
            extern int storage_save_all(void);
            storage_save_all();

            char audit_desc[128];
            snprintf(audit_desc, sizeof(audit_desc), "Disconnected power supply for %s", target->id);
            audit_log("POWER_DISCONNECTED", audit_desc);

            ui_message_box("Power Disconnected", "Consumer supply marked DISCONNECTED in the grid ledger.", 1);
        }
    }
}
