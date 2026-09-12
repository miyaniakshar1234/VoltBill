/**
 * @file customer.c
 * @brief Implementation of customer registration, search, and profile rendering.
 * @author Akshar Miyani (MCA 1st Sem, Manipal University Jaipur)
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

    printf("  " DBOX_TL);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_TR "\n");

    printf("  " DBOX_V "  " CLR_CYAN CLR_BOLD "%-12s" CLR_RESET "  " CLR_WHITE CLR_BOLD "%-44s" CLR_RESET " ", c->id, c->name);
    if (c->is_active) {
        printf(CLR_GREEN "[ACTIVE]" CLR_RESET "   " DBOX_V "\n");
    } else {
        printf(CLR_RED "[INACTIVE]" CLR_RESET " " DBOX_V "\n");
    }

    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    printf("  " DBOX_V "  " CLR_GRAY "Category    :" CLR_RESET " %-22s  " CLR_GRAY "Meter Number :" CLR_RESET " %-19s " DBOX_V "\n",
           category_to_string(c->category), c->meter_no);
    printf("  " DBOX_V "  " CLR_GRAY "Phase       :" CLR_RESET " %-22s  " CLR_GRAY "Contract Load:" CLR_RESET " %5.2f kW           " DBOX_V "\n",
           c->phase == PHASE_THREE ? "Three Phase (415V)" : "Single Phase (230V)", c->sanctioned_load_kw);
    printf("  " DBOX_V "  " CLR_GRAY "Phone       :" CLR_RESET " %-22s  " CLR_GRAY "Solar Rooftop:" CLR_RESET " %5.2f kW           " DBOX_V "\n",
           c->phone, c->solar_capacity_kw);
    printf("  " DBOX_V "  " CLR_GRAY "Email       :" CLR_RESET " %-22s  " CLR_GRAY "Registered   :" CLR_RESET " %-19s " DBOX_V "\n",
           c->email, c->registered_date);
    printf("  " DBOX_V "  " CLR_GRAY "Address     :" CLR_RESET " %-57s " DBOX_V "\n",
           c->address);

    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    char bal_str[32], cred_str[32];
    format_currency(c->outstanding_arrears, bal_str, sizeof(bal_str));
    format_currency(c->advance_credit, cred_str, sizeof(cred_str));

    printf("  " DBOX_V "  " CLR_GRAY "Unpaid Arrears :" CLR_RESET " " CLR_RED "%-18s" CLR_RESET "  " 
           CLR_GRAY "Advance Balance:" CLR_RESET " " CLR_GREEN "%-18s" CLR_RESET " " DBOX_V "\n",
           bal_str, cred_str);

    printf("  " DBOX_BL);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_BR "\n\n");
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
    get_current_date(c.registered_date, sizeof(c.registered_date));

    customer_add_record(&c);

    /* Save to storage */
    extern int storage_save_all(void);
    storage_save_all();

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

        printf("  " CLR_GRAY "│ " CLR_CYAN "%-8s" CLR_RESET CLR_GRAY "│ " 
               CLR_WHITE "%-24.24s" CLR_RESET CLR_GRAY "│ " 
               CLR_YELLOW "%-12s" CLR_RESET CLR_GRAY "│ " 
               CLR_GRAY "%-11s" CLR_RESET CLR_GRAY "│ " 
               CLR_WHITE "%7.2f " CLR_RESET CLR_GRAY "│ " 
               "%s%-9s" CLR_RESET CLR_GRAY "│" CLR_RESET "\n",
               c->id, c->name, cat_short, c->meter_no, c->sanctioned_load_kw,
               c->is_active ? CLR_GREEN : CLR_RED, c->is_active ? "Active" : "Inactive");
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

    /* Search by exact ID */
    Consumer *found = customer_find_by_id(query);
    if (found) {
        printf("\n  " CLR_GREEN "✓ Consumer ID match found:" CLR_RESET "\n\n");
        customer_render_card(found);
        pause_prompt();
        return;
    }

    /* Search by meter */
    found = customer_find_by_meter(query);
    if (found) {
        printf("\n  " CLR_GREEN "✓ Meter Serial match found:" CLR_RESET "\n\n");
        customer_render_card(found);
        pause_prompt();
        return;
    }

    /* Substring search in name */
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
        printf("  Status switched to: %s\n", c->is_active ? "ACTIVE" : "INACTIVE");
    }

    extern int storage_save_all(void);
    storage_save_all();

    ui_message_box("Profile Updated", "Consumer record updated successfully!", 1);
}
