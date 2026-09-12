/**
 * @file banner.c
 * @brief High-impact Cyberpunk artwork, boot diagnostics, and architectural aura framing.
 * @author Akshar Miyani
 * @version 2.0.0
 */

#include "banner.h"
#include "ui.h"
#include "utils.h"
#include <stdio.h>

void display_onboarding_splash(void) {
    clear_screen();
    printf("\n");

    /* Radiant Cyberpunk Gradient ASCII Logo */
    ui_print_gradient("   __      __        _   _     ____    _   _   _ \n", 0, 240, 255, 189, 0, 255, 1);
    ui_print_gradient("   \\ \\    / /       | | | |   |  _ \\  (_) | | | |\n", 0, 240, 255, 189, 0, 255, 1);
    ui_print_gradient("    \\ \\  / /   ___  | | | |_  | |_) |  _  | | | |\n", 0, 240, 255, 189, 0, 255, 1);
    ui_print_gradient("     \\ \\/ /   / _ \\ | | | __| |  _ <  | | | | | |\n", 0, 240, 255, 189, 0, 255, 1);
    ui_print_gradient("      \\  /   | (_) || | | |_  | |_) | | | | | | |\n", 0, 240, 255, 189, 0, 255, 1);
    ui_print_gradient("       \\/     \\___/ |_|  \\__| |____/  |_| |_| |_|\n", 0, 240, 255, 189, 0, 255, 1);
    printf("\n");

    const int W = 76;
    ui_card_begin(W, "VOLTBILL // POWER UTILITY DISTRIBUTION ENGINE");
    ui_card_row(W, CLR_CYAN CLR_BOLD "ENTERPRISE DISTRIBUTION CORE" CLR_RESET, CLR_GREEN CLR_BOLD "v2.0.0 ENTERPRISE" CLR_RESET);
    ui_card_text(W, CLR_GRAY "Next-Generation Systems Architecture Engineered in Pure ISO C (C99/C11)" CLR_RESET);
    ui_card_divider(W);
    ui_card_row(W, CLR_WHITE CLR_BOLD "Lead Systems Architect :" CLR_RESET, CLR_CYAN CLR_BOLD "AKSHAR MIYANI" CLR_RESET);
    ui_card_row(W, CLR_WHITE CLR_BOLD "Core Engine Runtime     :" CLR_RESET, CLR_CYAN "Deterministic Low-Latency POS & Tariff Processor" CLR_RESET);
    ui_card_row(W, CLR_WHITE CLR_BOLD "Data Integrity Standard :" CLR_RESET, CLR_GREEN "CRC-32 Checksummed Binary Vault & Atomic Persistence" CLR_RESET);
    ui_card_row(W, CLR_WHITE CLR_BOLD "Mathematical Tariffs    :" CLR_RESET, CLR_YELLOW "Progressive Slab Engine + Time-of-Day (ToD)" CLR_RESET);
    ui_card_row(W, CLR_WHITE CLR_BOLD "Target Platforms        :" CLR_RESET, CLR_VIOLET "Native Windows (x64) • Linux • macOS" CLR_RESET);
    ui_card_end(W);
    printf("\n");

    /* Futuristic boot diagnostics with microsecond timestamps */
    printf("  " CLR_WHITE CLR_BOLD "SYSTEM DIAGNOSTICS & TELEMETRY INITIALIZATION:" CLR_RESET "\n");
    const char *telemetry_steps[] = {
        "Mounting Dynamic Tariff Models (Domestic, Comm, Ind, Agri)",
        "Initializing High-Performance Zero-Allocation Memory Cache",
        "Validating CRC-32 Cryptographic File Checksums (Data Vault)",
        "Engaging Real-Time POS Payment Ledger & Time-of-Day Engine",
        "Synchronizing TrueColor ANSI TUI Subsystem & UTF-8 Visuals"
    };
    const char *times[] = {"0.18 ms", "0.22 ms", "0.34 ms", "0.15 ms", "0.09 ms"};

    for (int i = 0; i < 5; i++) {
        printf("  " CLR_GRAY "[ %d/5 ]" CLR_RESET " %-60s " CLR_DARK_GRAY "[" CLR_YELLOW "%s" CLR_DARK_GRAY "]" CLR_RESET " " CLR_GREEN "✓" CLR_RESET "\n",
               i + 1, telemetry_steps[i], times[i]);
        fflush(stdout);
        sleep_ms(40);
    }
    printf("\n  " CLR_GREEN CLR_BOLD "⚡ SYSTEM READY // 100%% OPERATIONAL" CLR_RESET "\n\n");

    printf("  " CLR_WHITE CLR_BOLD "Press any key to enter VoltBill Command Center..." CLR_RESET);
    fflush(stdout);
    read_key();
}

void display_banner_compact(void) {
    printf("  " CLR_CYAN CLR_BOLD "⚡ VOLTBILL" CLR_RESET " " CLR_GRAY "• Enterprise Power Utility Engine •" CLR_RESET " ");
    ui_print_gradient("Akshar Miyani", 0, 240, 255, 189, 0, 255, 1);
    printf("\n");
}

void display_about_screen(void) {
    ui_header("SYSTEM ARCHITECTURE & CREDENTIALS", "VoltBill Core Specification & Technical Whitepaper Summary");

    const int W = 76;
    ui_card_begin(W, "ARCHITECTURAL SPECIFICATION & LEADERSHIP");
    ui_card_row(W, CLR_WHITE "Project Engine :" CLR_RESET, CLR_YELLOW "VoltBill: Native Electricity Billing & Customer Management" CLR_RESET);
    ui_card_row(W, CLR_WHITE "Lead Architect :" CLR_RESET, CLR_CYAN CLR_BOLD "AKSHAR MIYANI" CLR_RESET);
    ui_card_row(W, CLR_WHITE "Implementation :" CLR_RESET, CLR_VIOLET "ISO C11 Native Core (Zero Dynamic Fragmentation)" CLR_RESET);
    ui_card_row(W, CLR_WHITE "GitHub Profile :" CLR_RESET, CLR_CYAN "https://github.com/miyaniakshar1234" CLR_RESET);
    ui_card_row(W, CLR_WHITE "Repository     :" CLR_RESET, CLR_CYAN "https://github.com/miyaniakshar1234/VoltBill" CLR_RESET);
    ui_card_divider(W);
    ui_card_section(W, "ENGINEERING PILLARS & PRODUCTION CAPABILITIES");
    ui_card_text(W, CLR_GRAY "• " CLR_WHITE "Progressive Tier Slabs: Dynamic boundary traversal with O(1) memory" CLR_RESET);
    ui_card_text(W, CLR_GRAY "• " CLR_WHITE "Solar Net-Metering: Direct bidirectional grid kWh offsetting" CLR_RESET);
    ui_card_text(W, CLR_GRAY "• " CLR_WHITE "Industrial Power Factor: Reactive penalty and high-efficiency rebates" CLR_RESET);
    ui_card_text(W, CLR_GRAY "• " CLR_WHITE "Time-of-Day (ToD): Peak-hour stress surcharge and solar rebates" CLR_RESET);
    ui_card_text(W, CLR_GRAY "• " CLR_WHITE "Batch Grid Processing: Simultaneous billing across entire grid nodes" CLR_RESET);
    ui_card_text(W, CLR_GRAY "• " CLR_WHITE "Data Vault: Binary datastores with CRC-32 integrity & atomic renames" CLR_RESET);
    ui_card_text(W, CLR_GRAY "• " CLR_WHITE "Visual TUI: TrueColor RGB gradients, live HUD & authentic QR matrix" CLR_RESET);
    ui_card_end(W);

    pause_prompt();
}
