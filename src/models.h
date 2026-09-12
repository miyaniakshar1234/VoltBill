/**
 * @file models.h
 * @brief Core data structures and enumerations for VoltBill.
 * @author Akshar Miyani (MCA 1st Sem, Manipal University Jaipur)
 * @version 1.0.0
 */

#ifndef MODELS_H
#define MODELS_H

#include <stddef.h>

#define MAX_SLABS 8
#define MAX_CONSUMERS 1000
#define MAX_READINGS 5000
#define MAX_BILLS 5000
#define MAX_PAYMENTS 5000

#define ID_LEN 16
#define NAME_LEN 64
#define PHONE_LEN 16
#define EMAIL_LEN 64
#define ADDR_LEN 128
#define METER_LEN 32
#define DATE_LEN 32
#define BILL_ID_LEN 24
#define REC_ID_LEN 24

/**
 * @brief Consumer connection category
 */
typedef enum {
    CAT_DOMESTIC = 0,    /* Residential households */
    CAT_COMMERCIAL,      /* Shops, malls, private offices */
    CAT_INDUSTRIAL,      /* Manufacturing units, heavy power */
    CAT_AGRICULTURAL     /* Irrigation pumps, farming connections */
} ConnectionCategory;

/**
 * @brief Electrical supply phase
 */
typedef enum {
    PHASE_SINGLE = 1,
    PHASE_THREE = 3
} PhaseType;

/**
 * @brief Status of an electricity bill
 */
typedef enum {
    BILL_PENDING = 0,
    BILL_PARTIALLY_PAID,
    BILL_PAID,
    BILL_OVERDUE
} BillStatus;

/**
 * @brief Payment transaction mode
 */
typedef enum {
    PAY_CASH = 0,
    PAY_UPI,
    PAY_CREDIT_DEBIT_CARD,
    PAY_NET_BANKING
} PaymentMode;

/**
 * @brief A single slab boundary and unit rate
 */
typedef struct {
    double min_units;       /* Slab lower bound in kWh (inclusive) */
    double max_units;       /* Slab upper bound in kWh (use -1 or 999999 for infinity) */
    double rate_per_unit;   /* Charge in ₹ per kWh */
} TariffSlab;

/**
 * @brief Tariff rules for a specific consumer category
 */
typedef struct {
    ConnectionCategory category;
    char category_name[32];
    int slab_count;
    TariffSlab slabs[MAX_SLABS];
    double fixed_charge_per_kw;        /* ₹ per kW sanctioned load per month */
    double meter_rent;                 /* ₹ fixed monthly meter maintenance */
    double regulatory_surcharge_pct;   /* % on energy charges (e.g., 3.8%) */
    double electricity_duty_pct;       /* % government tax (e.g., 5.0%) */
    double green_cess_per_unit;        /* ₹ per unit clean energy fund (e.g., ₹0.10) */
    double fppca_per_unit;             /* Fuel Surcharge Adjustment in ₹/unit */
    double prompt_rebate_pct;          /* % discount for payment before due date (e.g., 1.5%) */
    double late_penalty_pct;           /* % penalty surcharge for late payment (e.g., 2.0%) */
} TariffConfig;

/**
 * @brief Customer entity record
 */
typedef struct {
    char id[ID_LEN];                  /* Unique ID, e.g., "VB-1001" */
    char name[NAME_LEN];              /* Consumer Full Name */
    char phone[PHONE_LEN];            /* Mobile number */
    char email[EMAIL_LEN];            /* Email address */
    char address[ADDR_LEN];           /* Installation Address */
    char meter_no[METER_LEN];         /* Physical meter serial number */
    ConnectionCategory category;      /* Domestic, Commercial, Industrial, Agri */
    PhaseType phase;                  /* Single or Three phase */
    double sanctioned_load_kw;        /* Sanctioned contract load (kW) */
    double security_deposit;          /* Initial deposit in ₹ */
    double solar_capacity_kw;         /* Rooftop solar kW (0.0 if not installed) */
    double outstanding_arrears;       /* Unpaid balance from previous cycles in ₹ */
    double advance_credit;            /* Advance payments held in credit in ₹ */
    char registered_date[DATE_LEN];   /* YYYY-MM-DD */
    int is_active;                    /* 1 = active, 0 = disconnected */
} Consumer;

/**
 * @brief Meter reading cycle entry
 */
typedef struct {
    char reading_id[ID_LEN];          /* Unique reading identifier */
    char consumer_id[ID_LEN];         /* Associated consumer */
    char reading_date[DATE_LEN];      /* Reading date YYYY-MM-DD */
    double prev_reading;              /* Previous meter index (kWh) */
    double curr_reading;              /* Current meter index (kWh) */
    double units_consumed;            /* Raw units = curr - prev */
    double solar_exported_units;      /* Solar units pumped back to grid */
    double net_units;                 /* Billed units = consumed - solar */
    double power_factor;              /* Recorded power factor (0.50 - 1.00) */
    int is_meter_defective;           /* 1 if flagged defective */
} MeterReading;

/**
 * @brief Itemized electricity bill breakdown
 */
typedef struct {
    char bill_id[BILL_ID_LEN];        /* E.g. "BILL-202609-1001" */
    char consumer_id[ID_LEN];         /* Consumer identifier */
    char bill_date[DATE_LEN];         /* Billing issue date */
    char due_date[DATE_LEN];          /* Payment due date */
    char billing_cycle[16];           /* E.g. "SEP-2026" */
    
    /* Meter stats */
    double prev_reading;
    double curr_reading;
    double gross_units;
    double solar_units;
    double billed_units;
    double power_factor;

    /* Slabs and Energy Charge */
    int slab_breakdown_count;
    double slab_units[MAX_SLABS];
    double slab_rates[MAX_SLABS];
    double slab_amounts[MAX_SLABS];
    double total_energy_charges;

    /* Additional Levies */
    double fixed_charges;             /* Sanctioned load * rate */
    double meter_rent;                /* Meter hire */
    double regulatory_surcharge;      /* Regulatory charges */
    double electricity_duty;          /* Govt state duty */
    double green_cess;                /* Environmental cess */
    double fppca_charges;             /* Fuel price adjustment */
    double pf_penalty_or_rebate;      /* Industrial PF surcharge/discount */

    /* Final Totals */
    double current_cycle_total;       /* Sum of energy + all charges */
    double previous_arrears;          /* Brought forward arrears */
    double advance_adjusted;          /* Adjusted from credit */
    double net_payable_amount;        /* Total payable by due date */
    double prompt_payment_rebate;     /* Discount if paid before due date */
    double late_payment_surcharge;    /* Penalty if paid after due date */
    double amount_paid;               /* Amount settled so far */
    BillStatus status;                /* Pending, Partial, Paid, Overdue */
} BillBreakdown;

/**
 * @brief Payment transaction record
 */
typedef struct {
    char receipt_id[REC_ID_LEN];      /* E.g. "REC-202609-001" */
    char bill_id[BILL_ID_LEN];        /* Associated bill */
    char consumer_id[ID_LEN];         /* Consumer identifier */
    char payment_date[DATE_LEN];      /* Timestamp */
    double amount;                    /* Amount paid in ₹ */
    PaymentMode mode;                 /* Cash, UPI, Card, NetBanking */
    char transaction_ref[32];         /* Bank / UPI reference ID */
} PaymentRecord;

#endif /* MODELS_H */
