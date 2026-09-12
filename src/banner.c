/**
 * @file banner.c
 * @brief Implementation of ASCII artwork, onboarding sequence, and identity badges.
 * @author Akshar Miyani
 */

#include "banner.h"
#include "ui.h"
#include "utils.h"
#include <stdio.h>

void display_onboarding_splash(void) {
    clear_screen();
    printf("\n");
    printf("  " CLR_CYAN CLR_BOLD);
    printf("   __      __        _   _     ____    _   _   _ \n");
    printf("   \\ \\    / /       | | | |   |  _ \\  (_) | | | |\n");
    printf("    \\ \\  / /   ___  | | | |_  | |_) |  _  | | | |\n");
    printf("     \\ \\/ /   / _ \\ | | | __| |  _ <  | | | | | |\n");
    printf("      \\  /   | (_) || | | |_  | |_) | | | | | | |\n");
    printf("       \\/     \\___/ |_|  \\__| |____/  |_| |_| |_|\n");
    printf(CLR_RESET "\n");

    printf("  " DBOX_TL);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_TR "\n");

    printf("  " DBOX_V "  " CLR_YELLOW CLR_BOLD "VOLTBILL: ELECTRICITY BILLING & UTILITY ENGINE" CLR_RESET "           " CLR_CYAN "v1.1.0" CLR_RESET "  " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_GRAY "High-Performance Systems Utility Engine in ISO C (C99/C11)" CLR_RESET "           " DBOX_V "\n");
    
    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    printf("  " DBOX_V "  " CLR_WHITE CLR_BOLD "Lead Architect & Developer :" CLR_RESET "  " CLR_CYAN CLR_BOLD "AKSHAR MIYANI" CLR_RESET "                                 " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_WHITE CLR_BOLD "Core Engine Specification   :" CLR_RESET "  " CLR_VIOLET "Native Systems Utility & Tariffs Engine" CLR_RESET "       " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_WHITE CLR_BOLD "Standards & Architecture    :" CLR_RESET "  " CLR_GREEN "ISO C11 / Zero-Allocation Core Engine" CLR_RESET "          " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_WHITE CLR_BOLD "Distribution Channels       :" CLR_RESET "  " CLR_YELLOW "Windows (Winget/Scoop) • Linux (APT) • macOS" CLR_RESET "  " DBOX_V "\n");

    printf("  " DBOX_BL);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_BR "\n\n");

    /* Quick boot animation */
    printf("  " CLR_GRAY "⚡ Booting Core Engine: " CLR_RESET);
    const char *steps[] = {
        "Mounting Tariffs", "Verifying Schema", "Loading Datastores", "System Ready"
    };
    for (int i = 0; i < 4; i++) {
        printf(CLR_CYAN "[%s] " CLR_RESET, steps[i]);
        fflush(stdout);
        sleep_ms(60);
    }
    printf(CLR_GREEN "✓ OK" CLR_RESET "\n\n");

    printf("  " CLR_WHITE CLR_BOLD "Press any key to enter VoltBill Dashboard..." CLR_RESET);
    fflush(stdout);
    read_key();
}

void display_banner_compact(void) {
    printf("  " CLR_CYAN CLR_BOLD "⚡ VOLTBILL" CLR_RESET " " CLR_GRAY "• Electricity Billing Engine •" CLR_RESET " " 
           CLR_VIOLET "Akshar Miyani" CLR_RESET "\n");
}

void display_about_screen(void) {
    ui_header("ABOUT THE ARCHITECT & SYSTEM", "VoltBill Core Credits & Technical Specifications");

    printf("  " DBOX_TL);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_TR "\n");

    printf("  " DBOX_V "  " CLR_CYAN CLR_BOLD "⚡ PROJECT IDENTITY & DEVELOPER CREDENTIALS" CLR_RESET "                           " DBOX_V "\n");
    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    printf("  " DBOX_V "  " CLR_WHITE "Project Title   :" CLR_RESET "  " CLR_YELLOW "VoltBill: Electricity Billing & Utility Management System" CLR_RESET "  " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_WHITE "Lead Architect  :" CLR_RESET "  " CLR_CYAN CLR_BOLD "Akshar Miyani" CLR_RESET "                                            " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_WHITE "Language Standard:" CLR_RESET " " CLR_VIOLET "ISO C99 / ISO C11 Native Systems Core" CLR_RESET "                  " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_WHITE "GitHub Profile  :" CLR_RESET "  " CLR_WHITE "https://github.com/miyaniakshar1234" CLR_RESET "                     " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_WHITE "Source Repo     :" CLR_RESET "  " CLR_WHITE "https://github.com/miyaniakshar1234/VoltBill" CLR_RESET "            " DBOX_V "\n");

    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    printf("  " DBOX_V "  " CLR_CYAN CLR_BOLD "⚡ ARCHITECTURAL HIGHLIGHTS" CLR_RESET "                                            " DBOX_V "\n");
    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    printf("  " DBOX_V "  " CLR_GRAY "•" CLR_RESET " " CLR_WHITE "Progressive Multi-Tier Slab Calculation Engine (Domestic/Comm/Ind/Agri)" CLR_RESET " " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_GRAY "•" CLR_RESET " " CLR_WHITE "Solar Rooftop Net-Metering & Green Energy Export Offsets            " CLR_RESET " " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_GRAY "•" CLR_RESET " " CLR_WHITE "Industrial Power Factor Penalty and Surcharge Regulation            " CLR_RESET " " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_GRAY "•" CLR_RESET " " CLR_WHITE "Time-of-Day (ToD) Peak Hours Surcharge \u0026 Solar Off-Peak Rebates   " CLR_RESET " " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_GRAY "•" CLR_RESET " " CLR_WHITE "Dynamic Tariff Configuration via config/tariffs.cfg                  " CLR_RESET " " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_GRAY "•" CLR_RESET " " CLR_WHITE "High-Performance Binary Persistence & CSV Billing Ledger Exporter    " CLR_RESET " " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_GRAY "•" CLR_RESET " " CLR_WHITE "Visual Consumption Trends via UTF-8 Sparklines & CO2 Carbon Footprint " CLR_RESET " " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_GRAY "•" CLR_RESET " " CLR_WHITE "Deployable via Winget, Scoop, Homebrew, APT (.deb) & Standalone CLI  " CLR_RESET " " DBOX_V "\n");

    printf("  " DBOX_BL);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_BR "\n");

    pause_prompt();
}
