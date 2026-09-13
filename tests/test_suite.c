/**
 * @file test_suite.c
 * @brief Native Zero-Dependency Unit & Integration Test Suite for VoltBill
 * @author Akshar Miyani
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../src/models.h"
#include "../src/tariff.h"
#include "../src/billing.h"
#include "../src/customer.h"
#include "../src/qrcodegen.h"
#include "../src/analytics.h"

static int g_tests_run = 0;
static int g_tests_passed = 0;
static int g_tests_failed = 0;

#define ANSI_GREEN  "\033[38;2;0;255;163m"
#define ANSI_RED    "\033[38;2;255;75;75m"
#define ANSI_CYAN   "\033[38;2;0;240;255m"
#define ANSI_GRAY   "\033[38;2;140;140;160m"
#define ANSI_BOLD   "\033[1m"
#define ANSI_RESET  "\033[0m"

#define ASSERT_TRUE(expr, msg) do { \
    g_tests_run++; \
    if (expr) { \
        g_tests_passed++; \
        printf("  " ANSI_GREEN "✓ [PASS]" ANSI_RESET " %s\n", (msg)); \
    } else { \
        g_tests_failed++; \
        printf("  " ANSI_RED "✗ [FAIL]" ANSI_RESET " %s (" ANSI_RED "%s:%d" ANSI_RESET ")\n", (msg), __FILE__, __LINE__); \
    } \
} while(0)

#define ASSERT_NEAR(a, b, eps, msg) do { \
    g_tests_run++; \
    if (fabs((double)(a) - (double)(b)) <= (double)(eps)) { \
        g_tests_passed++; \
        printf("  " ANSI_GREEN "✓ [PASS]" ANSI_RESET " %s (got: %.2f, expected: %.2f)\n", (msg), (double)(a), (double)(b)); \
    } else { \
        g_tests_failed++; \
        printf("  " ANSI_RED "✗ [FAIL]" ANSI_RESET " %s (got: %.2f, expected: %.2f at %s:%d)\n", (msg), (double)(a), (double)(b), __FILE__, __LINE__); \
    } \
} while(0)

void test_tariff_defaults(void) {
    printf("\n" ANSI_CYAN ANSI_BOLD "[TEST SUITE 1: Tariff Configuration & Slab Bounds]" ANSI_RESET "\n");
    tariff_init();
    tariff_reset_defaults();

    TariffConfig *dom = tariff_get(CAT_DOMESTIC);
    ASSERT_TRUE(dom != NULL, "Domestic tariff configuration pointer is non-null");
    ASSERT_NEAR(dom->fixed_charge_per_kw, 50.0, 0.01, "Domestic fixed charge is Rs 50.0/kW");
    ASSERT_TRUE(dom->slab_count == 5, "Domestic has 5 progressive slabs");
    ASSERT_NEAR(dom->slabs[0].rate_per_unit, 3.15, 0.01, "Domestic Tier 1 rate is Rs 3.15/unit");
    ASSERT_NEAR(dom->slabs[1].rate_per_unit, 4.50, 0.01, "Domestic Tier 2 rate is Rs 4.50/unit");
    ASSERT_NEAR(dom->slabs[2].rate_per_unit, 6.20, 0.01, "Domestic Tier 3 rate is Rs 6.20/unit");
    ASSERT_NEAR(dom->slabs[3].rate_per_unit, 7.10, 0.01, "Domestic Tier 4 rate is Rs 7.10/unit");
    ASSERT_NEAR(dom->slabs[4].rate_per_unit, 7.95, 0.01, "Domestic Tier 5 rate is Rs 7.95/unit");

    TariffConfig *ind = tariff_get(CAT_INDUSTRIAL);
    ASSERT_TRUE(ind != NULL, "Industrial tariff configuration pointer is non-null");
    ASSERT_NEAR(ind->fixed_charge_per_kw, 200.0, 0.01, "Industrial fixed charge is Rs 200.0/kW");
}

void test_progressive_slab_billing(void) {
    printf("\n" ANSI_CYAN ANSI_BOLD "[TEST SUITE 2: Progressive Slab Calculus & Net-Metering]" ANSI_RESET "\n");

    Consumer c;
    memset(&c, 0, sizeof(c));
    strcpy(c.id, "TEST-DOM-01");
    strcpy(c.name, "Test Consumer");
    c.category = CAT_DOMESTIC;
    c.sanctioned_load_kw = 3.0;
    c.phase = PHASE_SINGLE;
    c.solar_capacity_kw = 0.0;

    BillBreakdown bill;
    memset(&bill, 0, sizeof(bill));

    /* Test Case A: 40 Units (Strictly Tier 1: 40 * 3.15 = 126.0) */
    billing_calculate(&c, 100.0, 140.0, 0.0, 0.0, 1.0, &bill);
    ASSERT_NEAR(bill.gross_units, 40.0, 0.01, "Gross consumption is 40.0 kWh");
    ASSERT_NEAR(bill.billed_units, 40.0, 0.01, "Net billed units is 40.0 kWh");
    ASSERT_NEAR(bill.total_energy_charges, 126.0, 0.01, "Energy charges strictly Tier 1 (40 * 3.15 = 126.0)");
    ASSERT_NEAR(bill.fixed_charges, 150.0, 0.01, "Fixed charge for 3 kW load (3 * 50 = 150.0)");

    /* Test Case B: 120 Units (50 in Tier 1 @ 3.15 = 157.5, 70 in Tier 2 @ 4.50 = 315.0 -> 472.5) */
    memset(&bill, 0, sizeof(bill));
    billing_calculate(&c, 1000.0, 1120.0, 0.0, 0.0, 1.0, &bill);
    ASSERT_NEAR(bill.billed_units, 120.0, 0.01, "Net billed units is 120.0 kWh");
    ASSERT_NEAR(bill.total_energy_charges, 472.50, 0.01, "Multi-tier progressive charges (50*3.15 + 70*4.50 = 472.5)");

    /* Test Case C: Solar Net-Metering (Gross 200, Solar Export 80 -> Net 120) */
    c.solar_capacity_kw = 5.0;
    memset(&bill, 0, sizeof(bill));
    billing_calculate(&c, 1000.0, 1200.0, 80.0, 0.0, 1.0, &bill);
    ASSERT_NEAR(bill.gross_units, 200.0, 0.01, "Gross units is 200.0 kWh");
    ASSERT_NEAR(bill.solar_units, 80.0, 0.01, "Solar export is 80.0 kWh");
    ASSERT_NEAR(bill.billed_units, 120.0, 0.01, "Net units after solar deduction is 120.0 kWh");
    ASSERT_NEAR(bill.total_energy_charges, 472.50, 0.01, "Energy charges computed exactly on net 120 units");
}

void test_power_factor_regulation(void) {
    printf("\n" ANSI_CYAN ANSI_BOLD "[TEST SUITE 3: Industrial Power Factor Regulation]" ANSI_RESET "\n");

    Consumer ind_c;
    memset(&ind_c, 0, sizeof(ind_c));
    strcpy(ind_c.id, "TEST-IND-01");
    strcpy(ind_c.name, "Precision Manufacturing Ltd");
    ind_c.category = CAT_INDUSTRIAL;
    ind_c.sanctioned_load_kw = 50.0;
    ind_c.phase = PHASE_THREE;

    BillBreakdown bill_low_pf;
    memset(&bill_low_pf, 0, sizeof(bill_low_pf));

    /* Lagging PF = 0.82 (< 0.90 threshold) -> Surcharge applied */
    billing_calculate(&ind_c, 0.0, 1000.0, 0.0, 0.0, 0.82, &bill_low_pf);
    ASSERT_TRUE(bill_low_pf.pf_penalty_or_rebate > 0.0, "Low power factor (0.82) applies positive penalty surcharge");

    BillBreakdown bill_high_pf;
    memset(&bill_high_pf, 0, sizeof(bill_high_pf));

    /* High PF = 0.98 (> 0.95 threshold) -> Incentive rebate applied */
    billing_calculate(&ind_c, 0.0, 1000.0, 0.0, 0.0, 0.98, &bill_high_pf);
    ASSERT_TRUE(bill_high_pf.pf_penalty_or_rebate < 0.0, "High power factor (0.98) applies negative rebate incentive");
}

void test_qr_engine(void) {
    printf("\n" ANSI_CYAN ANSI_BOLD "[TEST SUITE 4: ISO/IEC 18004 QR Code Generation Buffer Bounds]" ANSI_RESET "\n");

    const char *upi_uri = "upi://pay?pa=voltbill.utility@axisbank&pn=VoltBill%20Utility&am=1450.00&cu=INR";
    uint8_t qrcode[qrcodegen_BUFFER_LEN_FOR_VERSION(10)];
    uint8_t tempBuffer[qrcodegen_BUFFER_LEN_FOR_VERSION(10)];

    bool ok = qrcodegen_encodeText(upi_uri, tempBuffer, qrcode, qrcodegen_Ecc_MEDIUM,
                                  qrcodegen_VERSION_MIN, 10, qrcodegen_Mask_AUTO, true);
    ASSERT_TRUE(ok, "ISO/IEC 18004 QR matrix successfully encodes standard UPI URI");
    int size = qrcodegen_getSize(qrcode);
    ASSERT_TRUE(size >= 21 && size <= 57, "Generated QR matrix size within valid Version 1-10 bounds");
}

void test_struct_invariants(void) {
    printf("\n" ANSI_CYAN ANSI_BOLD "[TEST SUITE 5: Memory Footprint & Struct Layout Invariants]" ANSI_RESET "\n");

    ASSERT_TRUE(sizeof(Consumer) > 0 && sizeof(Consumer) <= 1024, "Consumer struct bounded <= 1024 bytes for cache alignment");
    ASSERT_TRUE(sizeof(BillBreakdown) > 0 && sizeof(BillBreakdown) <= 2048, "BillBreakdown struct bounded <= 2048 bytes");
    ASSERT_TRUE(sizeof(PaymentRecord) > 0 && sizeof(PaymentRecord) <= 512, "PaymentRecord struct bounded <= 512 bytes");
}

void test_ufls_algorithm(void) {
    printf("\n" ANSI_CYAN ANSI_BOLD "[TEST SUITE 6: Automated Under-Frequency Load Shedding (UFLS)]" ANSI_RESET "\n");

    UFLSResult res;

    /* Normal grid conditions (50.02 Hz, ROCOF 0.02) -> 0 shed */
    analytics_simulate_ufls(50.02, 0.02, &res);
    ASSERT_TRUE(res.stage_tripped == 0, "Normal grid frequency triggers Stage 0 (no trip)");
    ASSERT_NEAR(res.load_shed_kw, 0.0, 0.01, "Normal grid sheds 0 kW load");

    /* Stage 1 trigger (49.40 Hz, ROCOF 0.45 Hz/s) */
    analytics_simulate_ufls(49.40, 0.45, &res);
    ASSERT_TRUE(res.stage_tripped == 1, "Under-frequency 49.4 Hz triggers Stage 1 shedding");
    ASSERT_TRUE(res.feeders_tripped == 2, "Stage 1 trips 2 agricultural feeders");
    ASSERT_TRUE(res.load_shed_kw > 0.0, "Stage 1 sheds positive kW load");

    /* Stage 2 trigger (49.10 Hz, ROCOF 0.85 Hz/s) */
    analytics_simulate_ufls(49.10, 0.85, &res);
    ASSERT_TRUE(res.stage_tripped == 2, "Under-frequency 49.1 Hz triggers Stage 2 shedding");
    ASSERT_TRUE(res.feeders_tripped == 5, "Stage 2 trips 5 feeders");

    /* Stage 3 emergency (48.70 Hz, ROCOF 1.30 Hz/s) */
    analytics_simulate_ufls(48.70, 1.30, &res);
    ASSERT_TRUE(res.stage_tripped == 3, "Critical under-frequency 48.7 Hz triggers Stage 3 island emergency");
    ASSERT_TRUE(res.feeders_tripped == 9, "Stage 3 trips 9 feeders to prevent grid blackout");
    ASSERT_TRUE(res.recovered_freq_hz > 49.80, "Stage 3 successfully projects frequency restabilization > 49.80 Hz");
}

int main(void) {
    printf("\n=========================================================================\n");
    printf("  " ANSI_CYAN ANSI_BOLD "⚡ VoltBill Native C Regression & Integration Test Engine" ANSI_RESET "\n");
    printf("  " ANSI_GRAY "Lead Architect: Akshar Miyani" ANSI_RESET "\n");
    printf("=========================================================================\n");

    test_tariff_defaults();
    test_progressive_slab_billing();
    test_power_factor_regulation();
    test_qr_engine();
    test_struct_invariants();
    test_ufls_algorithm();

    printf("\n=========================================================================\n");
    printf("  " ANSI_BOLD "TEST RESULTS SUMMARY:" ANSI_RESET "\n");
    printf("  Total Checks Executed : %d\n", g_tests_run);
    printf("  Passed                : " ANSI_GREEN "%d" ANSI_RESET "\n", g_tests_passed);
    printf("  Failed                : " ANSI_RED "%d" ANSI_RESET "\n", g_tests_failed);
    printf("=========================================================================\n\n");

    if (g_tests_failed == 0) {
        printf("  " ANSI_GREEN ANSI_BOLD "✓ ALL REGRESSION TESTS PASSED CLEANLY (100%% SUCCESS)" ANSI_RESET "\n\n");
        return 0;
    } else {
        printf("  " ANSI_RED ANSI_BOLD "✗ SOME TESTS FAILED" ANSI_RESET "\n\n");
        return 1;
    }
}
