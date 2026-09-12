/**
 * @file main.c
 * @brief Application entry point, interactive dispatcher, and CLI engine for VoltBill.
 * @author Akshar Miyani (MCA 1st Sem, Manipal University Jaipur)
 * @version 1.0.0
 */

#include "models.h"
#include "banner.h"
#include "ui.h"
#include "utils.h"
#include "tariff.h"
#include "customer.h"
#include "billing.h"
#include "payment.h"
#include "analytics.h"
#include "storage.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Submenus */

static void menu_customer_management(void) {
    const char *opts[] = {
        "Register New Customer Connection",
        "View All Registered Consumers",
        "Search Consumer Profile (ID / Name / Meter)",
        "Edit Consumer Details / Connection",
        "Return to Main Dashboard"
    };

    while (1) {
        int choice = ui_menu("CUSTOMER MANAGEMENT MODULE", opts, 5, 0);
        if (choice < 0 || choice == 4) break;

        switch (choice) {
            case 0: customer_register_flow(); break;
            case 1: customer_list_all(); break;
            case 2: customer_search_flow(); break;
            case 3: customer_edit_flow(); break;
            default: break;
        }
    }
}

static void menu_billing_management(void) {
    const char *opts[] = {
        "Record Meter Reading & Generate Bill",
        "View All Issued Bills & Payment Status",
        "Inspect Specific Invoice by Bill ID",
        "Return to Main Dashboard"
    };

    while (1) {
        int choice = ui_menu("METER READING & BILLING ENGINE", opts, 4, 0);
        if (choice < 0 || choice == 3) break;

        switch (choice) {
            case 0: billing_generate_flow(); break;
            case 1: billing_list_all(); break;
            case 2: {
                printf("  " CLR_WHITE "Enter Bill ID: " CLR_RESET);
                char bid[32];
                get_safe_string(bid, sizeof(bid));
                BillBreakdown *b = billing_find_by_id(bid);
                if (b) {
                    Consumer *c = customer_find_by_id(b->consumer_id);
                    clear_screen();
                    billing_render_invoice(b, c);
                    pause_prompt();
                } else {
                    ui_message_box("Not Found", "Bill ID was not found.", 0);
                }
                break;
            }
            default: break;
        }
    }
}

static void menu_payment_management(void) {
    const char *opts[] = {
        "Collect Payment & Issue Settlement Receipt",
        "Audit Log of All Payment Transactions",
        "Return to Main Dashboard"
    };

    while (1) {
        int choice = ui_menu("PAYMENT & LEDGER SETTLEMENT", opts, 3, 0);
        if (choice < 0 || choice == 2) break;

        switch (choice) {
            case 0: payment_process_flow(); break;
            case 1: payment_list_all(); break;
            default: break;
        }
    }
}

static void menu_analytics(void) {
    const char *opts[] = {
        "Grid Overview: Revenue, Load & Sector Charts",
        "Consumer Deep-Dive: Historical Trends & Carbon Footprint",
        "Return to Main Dashboard"
    };

    while (1) {
        int choice = ui_menu("ANALYTICS & ENVIRONMENTAL INTELLIGENCE", opts, 3, 0);
        if (choice < 0 || choice == 2) break;

        switch (choice) {
            case 0: analytics_system_overview(); break;
            case 1: analytics_consumer_deepdive(); break;
            default: break;
        }
    }
}

static void menu_tariffs(void) {
    const char *opts[] = {
        "View Active Slab Tariffs & Government Taxes",
        "Modify Slab Rates & Regulatory Charges",
        "Reset Tariffs to Default Slabs",
        "Return to Main Dashboard"
    };

    while (1) {
        int choice = ui_menu("DYNAMIC TARIFF CONFIGURATION", opts, 4, 0);
        if (choice < 0 || choice == 3) break;

        switch (choice) {
            case 0: tariff_display_all(); break;
            case 1: tariff_edit_menu(); break;
            case 2:
                tariff_reset_defaults();
                tariff_save_to_file();
                ui_message_box("Tariffs Reset", "Restored default rates successfully!", 1);
                break;
            default: break;
        }
    }
}

static void menu_storage_tools(void) {
    const char *opts[] = {
        "Seed Professor Evaluation Demo Records (6 Profiles)",
        "Export All Data to CSV Spreadsheets (Excel)",
        "Database Diagnostics & File Status",
        "Return to Main Dashboard"
    };

    while (1) {
        int choice = ui_menu("SYSTEM TOOLS & DATA MANAGEMENT", opts, 4, 0);
        if (choice < 0 || choice == 3) break;

        switch (choice) {
            case 0: storage_seed_demo_data(); break;
            case 1:
                storage_export_csv();
                ui_message_box("CSV Exported", "Generated data/export_consumers.csv and data/export_bills.csv", 1);
                break;
            case 2: storage_display_stats(); break;
            default: break;
        }
    }
}

static void print_version(void) {
    printf("VoltBill v1.0.0\n");
    printf("Lead Architect & Developer : Akshar Miyani\n");
    printf("Program & Academic Year    : MCA (1st Semester)\n");
    printf("Institution                : Manipal University Jaipur (MUJ)\n");
    printf("Subject                    : C Programming & Systems Fundamentals\n");
    printf("Repository                 : https://github.com/miyaniakshar1234/VoltBill\n");
}

static void print_help(void) {
    printf("VoltBill - Electricity Bill Management System in C\n");
    printf("Usage: voltbill [OPTIONS]\n\n");
    printf("Options:\n");
    printf("  --demo, --seed       Seed realistic evaluation demo records & launch\n");
    printf("  --export             Export all consumers and bills to CSV and exit\n");
    printf("  --version, -v        Display developer info and version\n");
    printf("  --help, -h           Show this help message\n");
}

int main(int argc, char *argv[]) {
    init_console();
    tariff_init();
    storage_init();

    /* Check CLI arguments */
    if (argc > 1) {
        if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
            print_version();
            restore_console();
            return 0;
        }
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            print_help();
            restore_console();
            return 0;
        }
        if (strcmp(argv[1], "--export") == 0) {
            storage_export_csv();
            printf("VoltBill: Exported CSV files to data/ directory.\n");
            restore_console();
            return 0;
        }
        if (strcmp(argv[1], "--demo") == 0 || strcmp(argv[1], "--seed") == 0) {
            storage_seed_demo_data();
        }
    } else {
        /* If freshly launched with no consumers, show splash banner! */
        display_onboarding_splash();
    }

    const char *main_menu_options[] = {
        "Customer Registration & Profiles",
        "Meter Reading & Bill Generation",
        "Payment Processing & Receipts",
        "Grid Analytics & Carbon Footprint",
        "Dynamic Tariff & Slab Rules",
        "System Tools, Seeder & CSV Export",
        "About Developer & MUJ Credentials",
        "Save & Exit VoltBill Engine"
    };

    while (1) {
        int sel = ui_menu("MAIN CONTROL DASHBOARD", main_menu_options, 8, 0);

        if (sel == 0) {
            menu_customer_management();
        } else if (sel == 1) {
            menu_billing_management();
        } else if (sel == 2) {
            menu_payment_management();
        } else if (sel == 3) {
            menu_analytics();
        } else if (sel == 4) {
            menu_tariffs();
        } else if (sel == 5) {
            menu_storage_tools();
        } else if (sel == 6) {
            display_about_screen();
        } else if (sel == 7 || sel < 0) {
            /* Exit */
            storage_save_all();
            storage_export_csv();
            clear_screen();
            printf("\n");
            printf("  " CLR_CYAN CLR_BOLD "⚡ VoltBill Session Terminated." CLR_RESET "\n");
            printf("  " CLR_WHITE "All data saved to persistent datastores successfully." CLR_RESET "\n");
            printf("  " CLR_GRAY "Thank you for evaluating VoltBill • Akshar Miyani (MCA 1st Sem, MUJ)" CLR_RESET "\n\n");
            break;
        }
    }

    restore_console();
    return 0;
}
