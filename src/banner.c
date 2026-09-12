/**
 * @file banner.c
 * @brief Implementation of ASCII artwork, onboarding sequence, and identity badges.
 * @author Akshar Miyani (MCA 1st Sem, Manipal University Jaipur)
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

    printf("  " DBOX_V "  " CLR_YELLOW CLR_BOLD "VOLTBILL: ELECTRICITY BILLING & UTILITY ENGINE" CLR_RESET "           " CLR_CYAN "v1.0.0" CLR_RESET "  " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_GRAY "Academic Systems Programming Project in ISO C (C99/C11)" CLR_RESET "                 " DBOX_V "\n");
    
    printf("  " DBOX_T_RIGHT);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_T_LEFT "\n");

    printf("  " DBOX_V "  " CLR_WHITE CLR_BOLD "Architect & Developer :" CLR_RESET "  " CLR_CYAN CLR_BOLD "AKSHAR MIYANI" CLR_RESET "                                       " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_WHITE CLR_BOLD "Academic Program       :" CLR_RESET "  " CLR_VIOLET "Master of Computer Applications (MCA - 1st Sem)" CLR_RESET "   " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_WHITE CLR_BOLD "Institution            :" CLR_RESET "  " CLR_GREEN "Manipal University Jaipur (MUJ)" CLR_RESET "                   " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_WHITE CLR_BOLD "Subject / Course       :" CLR_RESET "  " CLR_WHITE "Advanced C Programming & Systems Fundamentals" CLR_RESET "    " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_WHITE CLR_BOLD "Distribution Package   :" CLR_RESET "  " CLR_YELLOW "Cross-Platform (Windows • Linux • macOS)" CLR_RESET "          " DBOX_V "\n");

    printf("  " DBOX_BL);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_BR "\n\n");

    /* Quick boot animation */
    printf("  " CLR_GRAY "⚡ Initializing Core Engine: " CLR_RESET);
    const char *steps[] = {
        "Loading Tariffs", "Verifying Data Schemas", "Mounting Storage", "System Ready"
    };
    for (int i = 0; i < 4; i++) {
        printf(CLR_CYAN "[%s] " CLR_RESET, steps[i]);
        fflush(stdout);
        sleep_ms(70);
    }
    printf(CLR_GREEN "✓ OK" CLR_RESET "\n\n");

    printf("  " CLR_WHITE CLR_BOLD "Press any key to enter VoltBill Dashboard..." CLR_RESET);
    fflush(stdout);
    read_key();
}

void display_banner_compact(void) {
    printf("  " CLR_CYAN CLR_BOLD "⚡ VOLTBILL" CLR_RESET " " CLR_GRAY "• Electricity Billing Engine •" CLR_RESET " " 
           CLR_VIOLET "Akshar Miyani (MCA, MUJ)" CLR_RESET "\n");
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
    printf("  " DBOX_V "  " CLR_WHITE "Degree / Year   :" CLR_RESET "  " CLR_VIOLET "MCA (Master of Computer Applications) - 1st Semester" CLR_RESET "     " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_WHITE "University      :" CLR_RESET "  " CLR_GREEN "Manipal University Jaipur (MUJ)" CLR_RESET "                          " DBOX_V "\n");
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
    printf("  " DBOX_V "  " CLR_GRAY "•" CLR_RESET " " CLR_WHITE "Solar Rooftop Net-Metering & Green Energy Export Calculations         " CLR_RESET " " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_GRAY "•" CLR_RESET " " CLR_WHITE "Industrial Power Factor Penalty and Surcharge Regulation            " CLR_RESET " " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_GRAY "•" CLR_RESET " " CLR_WHITE "Dynamic Tariff Configuration via config/tariffs.cfg                  " CLR_RESET " " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_GRAY "•" CLR_RESET " " CLR_WHITE "High-Performance Binary Persistence & CSV Billing Ledger Exporter    " CLR_RESET " " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_GRAY "•" CLR_RESET " " CLR_WHITE "Visual Consumption Trends via UTF-8 Sparklines & CO2 Carbon Footprint " CLR_RESET " " DBOX_V "\n");
    printf("  " DBOX_V "  " CLR_GRAY "•" CLR_RESET " " CLR_WHITE "Deployable via Winget, Scoop, Homebrew, APT (.deb) & Standalone CLI  " CLR_RESET " " DBOX_V "\n");

    printf("  " DBOX_BL);
    for (int i = 0; i < 74; i++) printf(DBOX_H);
    printf(DBOX_BR "\n");

    pause_prompt();
}
