/**
 * @file main.c
 * @brief Application entry point, interactive dispatcher, and CLI engine for VoltBill.
 * @author Akshar Miyani
 * @version 1.1.0
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
        "Defaulters Ledger & Disconnection Notices",
        "Return to Main Dashboard"
    };

    while (1) {
        int choice = ui_menu("CUSTOMER MANAGEMENT MODULE", opts, 6, 0);
        if (choice < 0 || choice == 5) break;

        switch (choice) {
            case 0: customer_register_flow(); break;
            case 1: customer_list_all(); break;
            case 2: customer_search_flow(); break;
            case 3: customer_edit_flow(); break;
            case 4: customer_defaulters_flow(); break;
            default: break;
        }
    }
}

static void menu_billing_management(void) {
    const char *opts[] = {
        "Record Meter Reading & Generate Bill",
        "Batch Grid Billing Run (All Active Consumers)",
        "View All Issued Bills & Payment Status",
        "Filter & Query Invoices (Paid / Unpaid / Consumer)",
        "Inspect Specific Invoice by Bill ID",
        "Instant Tariff & What-If Bill Simulator",
        "Return to Main Dashboard"
    };

    while (1) {
        int choice = ui_menu("METER READING & BILLING ENGINE", opts, 7, 0);
        if (choice < 0 || choice == 6) break;

        switch (choice) {
            case 0: billing_generate_flow(); break;
            case 1: billing_batch_generate_flow(); break;
            case 2: billing_list_all(); break;
            case 3: billing_filter_flow(); break;
            case 4: {
                printf("  " CLR_WHITE "Enter Bill ID: " CLR_RESET);
                char bid[32];
                get_safe_string(bid, sizeof(bid));
                BillBreakdown *b = billing_find_by_id(bid);
                if (b) {
                    Consumer *c = customer_find_by_id(b->consumer_id);
                    clear_screen();
                    billing_render_invoice(b, c);
                    printf("  " CLR_CYAN "Quick Actions:" CLR_RESET "\n");
                    printf("    [1] View Fullscreen Scannable QR Code Matrix\n");
                    printf("    [0] Return to Billing Menu\n\n");
                    int qact = get_safe_int("  Select Action [0-1]: ", 0, 1);
                    if (qact == 1) {
                        char qr_link[256];
                        snprintf(qr_link, sizeof(qr_link), "upi://pay?pa=voltbill.utility@axisbank&pn=VoltBill%%20Utility&am=%.2f&cu=INR&tn=%s",
                                 b->net_payable_amount, b->bill_id);
                        char sub[128];
                        snprintf(sub, sizeof(sub), "Bill: %s ◈ Consumer: %s ◈ Amount: Rs. %.2f",
                                 b->bill_id, b->consumer_id, b->net_payable_amount);
                        ui_render_fullscreen_qr("INSTANT DIGITAL UPI SETTLEMENT", qr_link, sub);
                    }
                } else {
                    ui_message_box("Not Found", "Bill ID was not found.", 0);
                }
                break;
            }
            case 5: billing_simulator_flow(); break;
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
        "Real-Time SCADA Substation & Grid Telemetry Monitor",
        "Return to Main Dashboard"
    };

    while (1) {
        int choice = ui_menu("ANALYTICS & ENVIRONMENTAL INTELLIGENCE", opts, 4, 0);
        if (choice < 0 || choice == 3) break;

        switch (choice) {
            case 0: analytics_system_overview(); break;
            case 1: analytics_consumer_deepdive(); break;
            case 2: analytics_scada_grid_monitor(); pause_prompt(); break;
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
                audit_log("TARIFF_RESET", "Restored default rates");
                ui_message_box("Tariffs Reset", "Restored default rates successfully!", 1);
                break;
            default: break;
        }
    }
}

static void menu_storage_tools(void) {
    const char *opts[] = {
        "Seed Evaluation Demo Records (6 Diverse Profiles)",
        "Create Full Database Backup Snapshot (JSON)",
        "Export All Data to CSV Spreadsheets (Excel)",
        "Database Diagnostics & File Status",
        "Return to Main Dashboard"
    };

    while (1) {
        int choice = ui_menu("SYSTEM TOOLS & DATA MANAGEMENT", opts, 5, 0);
        if (choice < 0 || choice == 4) break;

        switch (choice) {
            case 0: storage_seed_demo_data(); break;
            case 1: {
                char backup_path[128];
                if (storage_create_backup(backup_path, sizeof(backup_path))) {
                    char msg[256];
                    snprintf(msg, sizeof(msg), "Backup created at %s", backup_path);
                    ui_message_box("Backup Complete", msg, 1);
                } else {
                    ui_message_box("Backup Failed", "Unable to create backup snapshot.", 0);
                }
                break;
            }
            case 2:
                storage_export_csv();
                audit_log("CSV_EXPORT", "Exported consumers and bills to CSV");
                ui_message_box("CSV Exported", "Generated data/export_consumers.csv and data/export_bills.csv", 1);
                break;
            case 3: storage_display_stats(); break;
            default: break;
        }
    }
}

static void print_version(void) {
    printf("VoltBill v2.0.0 (Cyber-Volt Enterprise Edition)\n");
    printf("Lead Systems Architect : Akshar Miyani\n");
    printf("Platform               : Native High-Performance Systems Core (ISO C11)\n");
    printf("Repository             : https://github.com/miyaniakshar1234/VoltBill\n");
}

static void print_help(void) {
    printf("VoltBill - Electricity Bill Management System in C\n");
    printf("Lead Architect: Akshar Miyani\n\n");
    printf("Usage: voltbill [OPTIONS] [COMMAND] [ARGS...]\n\n");
    printf("Options:\n");
    printf("  --demo, --seed                   Seed realistic demo records & launch dashboard\n");
    printf("  --export                         Export all consumers and bills to CSV and exit\n");
    printf("  --backup                         Create a full timestamped JSON backup snapshot\n");
    printf("  --batch                          Execute batch billing run for all active consumers\n");
    printf("  --version, -v                    Display developer info and version\n");
    printf("  --help, -h                       Show this help message\n\n");
    printf("CLI Scripting Commands:\n");
    printf("  voltbill bill <id> <reading>     Generate invoice directly from terminal\n");
    printf("  voltbill pay <id> <amount> [m]   Process payment (mode 0=Cash, 1=UPI, 2=Card, 3=NetBank)\n");
    printf("  voltbill status <id>             Show instant balance and meter status\n");
    printf("  voltbill calc <units> [cat] [sol]Instant tariff simulation (0=Dom, 1=Comm, 2=Ind, 3=Agri)\n");
    printf("  voltbill qr <payload>            Synthesize & display real scannable QR matrix\n");
    printf("  voltbill scada, voltbill grid    Launch real-time SCADA substation grid monitor\n");
}

int main(int argc, char *argv[]) {
    init_console();
    tariff_init();
    storage_init();

    /* Check CLI arguments and subcommands */
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
        if (strcmp(argv[1], "--backup") == 0) {
            char bpath[128];
            if (storage_create_backup(bpath, sizeof(bpath))) {
                printf("VoltBill: Database backup saved to %s\n", bpath);
            }
            restore_console();
            return 0;
        }
        if (strcmp(argv[1], "--batch") == 0) {
            billing_batch_generate_flow();
            restore_console();
            return 0;
        }
        if (strcmp(argv[1], "--demo") == 0 || strcmp(argv[1], "--seed") == 0) {
            storage_seed_demo_data();
        }
        /* Scripting Subcommand: voltbill calc <units> [category] [solar] */
        if (strcmp(argv[1], "calc") == 0 && argc >= 3) {
            double units = atof(argv[2]);
            int cat = (argc >= 4) ? atoi(argv[3]) : 0;
            double solar = (argc >= 5) ? atof(argv[4]) : 0.0;
            billing_quick_calc(units, cat, solar);
            restore_console();
            return 0;
        }
        /* Scripting Subcommand: voltbill bill <consumer-id> <curr-reading> */
        if (strcmp(argv[1], "bill") == 0 && argc >= 4) {
            const char *cid = argv[2];
            double reading = atof(argv[3]);
            billing_quick_bill(cid, reading);
            restore_console();
            return 0;
        }
        /* Scripting Subcommand: voltbill pay <target> <amount> [mode] */
        if (strcmp(argv[1], "pay") == 0 && argc >= 4) {
            const char *target = argv[2];
            double amount = atof(argv[3]);
            int mode = (argc >= 5) ? atoi(argv[4]) : 1;
            payment_quick_pay(target, amount, mode);
            restore_console();
            return 0;
        }
        /* Scripting Subcommand: voltbill status <consumer-id> */
        if (strcmp(argv[1], "status") == 0 && argc >= 3) {
            customer_quick_status(argv[2]);
            restore_console();
            return 0;
        }
        /* Scripting Subcommand: voltbill qr <text/url> */
        if (strcmp(argv[1], "qr") == 0 && argc >= 3) {
            const char *qr_data = argv[2];
            ui_render_fullscreen_qr("VOLTBILL TERMINAL QR SYNTHESIZER", qr_data, "Scannable Standard ISO/IEC 18004 2D Barcode");
            restore_console();
            return 0;
        }
        /* Scripting Subcommand: voltbill scada / grid */
        if (strcmp(argv[1], "scada") == 0 || strcmp(argv[1], "grid") == 0) {
            analytics_scada_grid_monitor();
            restore_console();
            return 0;
        }
    } else {
        /* If freshly launched with no consumers, show splash banner */
        display_onboarding_splash();
    }

    const char *main_menu_options[] = {
        "Customer Registration & Profiles",
        "Meter Reading & Bill Generation",
        "Payment Processing & Receipts",
        "Grid Analytics & Carbon Footprint",
        "Dynamic Tariff & Slab Rules",
        "System Tools, Seeder & CSV Export",
        "About Developer & Architecture",
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
            storage_save_all();
            storage_export_csv();
            clear_screen();
            printf("\n");
            printf("  " CLR_CYAN CLR_BOLD "⚡ VoltBill Session Terminated." CLR_RESET "\n");
            printf("  " CLR_WHITE "All data saved to persistent datastores successfully." CLR_RESET "\n");
            printf("  " CLR_GRAY "Thank you for using VoltBill • Akshar Miyani" CLR_RESET "\n\n");
            break;
        }
    }

    restore_console();
    return 0;
}
