<div align="center">

```
   __      __        _   _     ____    _   _   _ 
   \ \    / /       | | | |   |  _ \  (_) | | | |
    \ \  / /   ___  | | | |_  | |_) |  _  | | | |
     \ \/ /   / _ \ | | | __| |  _ <  | | | | | |
      \  /   | (_) || | | |_  | |_) | | | | | | |
       \/     \___/ |_|  \__| |____/  |_| |_| |_|
```

# ⚡ VoltBill
### A High-Performance Console-Based Electricity Billing & Utility Management System in ISO C

[![C Standard](https://img.shields.io/badge/C-C99%20%2F%20C11-00F0FF.svg?style=for-the-badge&logo=c)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-BD00FF.svg?style=for-the-badge)](https://github.com/miyaniakshar1234/VoltBill)
[![License: MIT](https://img.shields.io/badge/License-MIT-00FFA3.svg?style=for-the-badge)](LICENSE)
[![Academic](https://img.shields.io/badge/MUJ-MCA%201st%20Semester-FFE600.svg?style=for-the-badge)](https://jaipur.manipal.edu/)

**Architect & Lead Developer:** **Akshar Miyani**  
**Program:** Master of Computer Applications (MCA - 1st Semester)  
**Institution:** Manipal University Jaipur (MUJ)  
**Subject:** Advanced C Programming & Computer Systems

---

</div>

## 📌 Executive Summary

**VoltBill** re-imagines traditional academic billing projects into an enterprise-grade utility distribution engine written in native ISO C. Combining a **cyber-aesthetic ANSI terminal interface**, **multi-tier progressive slab calculations**, **solar rooftop net-metering offsets**, **industrial power factor regulation**, **runtime-configurable tariffs**, and **Unicode-based consumption analytics**, VoltBill serves as both a high-scoring academic showcase and a deployable native CLI utility.

---

## ✨ Powerhouse Feature Matrix

| Feature Domain | Capabilities & Engineering Highlights |
| :--- | :--- |
| **👤 Customer Management** | Unique alphanumeric Consumer IDs (`VB-1001`), meter serial tracking, contact metadata, connection categorization (Domestic, Commercial, Industrial, Agricultural), phase selection (Single/3-Phase), and contract load in kW. |
| **⚡ Meter Reading & Ingestion** | Previous & current reading validation, meter dial rollover recovery, defective meter flagging, and suspicious consumption anomaly alerts. |
| **☀️ Green Solar Net-Metering** | Deducts solar energy exported back to the grid from gross grid consumption, automatically calculating environmental savings and net billed units. |
| **📈 Progressive Slab Engine** | Multi-tier slab billing across categories (e.g. Domestic 0-50 @ ₹3.15, 51-150 @ ₹4.50, 151-300 @ ₹6.20, >300 @ ₹7.50). |
| **🏭 Industrial Power Factor** | Enforces lagging power factor penalty surcharges (PF < 0.90) and incentives (PF > 0.95) to protect grid stability. |
| **🧾 Tax & Surcharge Levies** | Computes Sanctioned Load Fixed Charges, Meter Rent, Regulatory Asset Surcharge, State Electricity Duty, Green Energy Cess, and Fuel Price Adjustment (FPPCA). |
| **📊 Visual Terminal Analytics** | Renders horizontal energy consumption bar charts using UTF-8 blocks (` ▂▃▄▅▆▇█`), calculates daily burn rates, and projects next month's consumption. |
| **🌱 Carbon Footprint Engine** | Grounded grid greenhouse emission calculator ($0.82 \text{ kg } CO_2\text{/kWh}$) calculating carbon output and mature trees needed to offset. |
| **💳 Payment & Audit Ledger** | Supports Cash, UPI, Credit/Debit Cards, and NetBanking. Generates verified receipt numbers (`REC-xxxx`), tracks arrears, and carries forward advance credit. |
| **⚙️ Dynamic Tariff Config** | Runtime editable tariffs stored in `config/tariffs.cfg` — modify slab rates and government taxes without recompiling C code! |
| **💾 Data Persistence & CSV** | High-performance binary datastores (`data/*.dat`) + automated CSV spreadsheet exporter (`data/export_*.csv`) for Microsoft Excel or Google Sheets. |
| **🎓 1-Click Professor Demo** | Built-in data seeder (`voltbill --demo`) instantly creates 6 diverse multi-category consumers with 3 historical billing cycles. |

---

## 🚀 Quick Installation & Distribution

VoltBill is cross-platform and can be installed via package managers or single-line commands:

### 🪟 Windows (PowerShell One-Liner)
Open PowerShell and run:
```powershell
irm https://raw.githubusercontent.com/miyaniakshar1234/VoltBill/main/install.ps1 | iex
```

### 🐧 Linux & 🍎 macOS (Shell One-Liner)
Open Terminal and run:
```bash
curl -fsSL https://raw.githubusercontent.com/miyaniakshar1234/VoltBill/main/install.sh | bash
```

### 📦 Windows Package Managers

#### Via Scoop:
```powershell
scoop install https://raw.githubusercontent.com/miyaniakshar1234/VoltBill/main/packaging/scoop/voltbill.json
```

#### Via Winget:
```powershell
winget install AksharMiyani.VoltBill
```

### 🍺 macOS (Homebrew Tap)
```bash
brew install miyaniakshar1234/tap/voltbill
```

---

## 🛠️ Building from Source

### Prerequisites:
- **Windows**: Visual Studio 2022 Build Tools (MSVC `cl.exe`) or MinGW GCC.
- **Linux**: `gcc`, `make`, and `cmake` (Ubuntu/Debian: `sudo apt install build-essential cmake`).
- **macOS**: Xcode Command Line Tools (`xcode-select --install`).

### 1-Click Build Scripts:
- **Windows (PowerShell)**:
  ```powershell
  .\build.ps1
  ```
- **Windows (CMD Batch)**:
  ```cmd
  build.bat
  ```
- **Linux / macOS (Make)**:
  ```bash
  make
  ./bin/voltbill
  ```
- **Cross-Platform (CMake)**:
  ```bash
  cmake -B build -DCMAKE_BUILD_TYPE=Release
  cmake --build build --config Release
  ```

---

## 💻 CLI Commands & Professor Evaluation Mode

VoltBill comes with built-in command line flags:

```bash
# Launch interactive TUI dashboard
voltbill

# 🌟 Instant Professor Demo Mode (seeds 6 sample consumers & multi-month bills)
voltbill --demo

# Export all database records to CSV spreadsheets and exit
voltbill --export

# Display student credentials, degree, and university attribution
voltbill --version

# Show CLI argument options
voltbill --help
```

---

## 🖥️ Terminal UI Preview

### 1. Main Control Dashboard
```
  VOLTBILL v1.0.0  │  MAIN CONTROL DASHBOARD
  Use Arrow Keys [↑/↓] or [1-9] to navigate, [Enter] to select, [Esc] to return
  ────────────────────────────────────────────────────────────────────────────

  ┌────────────────────────────────────────────────────────────────────────┐
  │   ► [1] Customer Registration & Profiles                               │
  │     [2] Meter Reading & Bill Generation                                │
  │     [3] Payment Processing & Receipts                                  │
  │     [4] Grid Analytics & Carbon Footprint                              │
  │     [5] Dynamic Tariff & Slab Rules                                    │
  │     [6] System Tools, Seeder & CSV Export                              │
  │     [7] About Developer & MUJ Credentials                              │
  │     [8] Save & Exit VoltBill Engine                                    │
  └────────────────────────────────────────────────────────────────────────┘
```

### 2. Itemized Final Bill & Tax Invoice
```
  ╔══════════════════════════════════════════════════════════════════════════╗
  ║  VOLTBILL STATE UTILITY DISTRIBUTION COMPANY                             ║
  ║  Academic Systems Engineering Project • Manipal University Jaipur (MUJ)  ║
  ║  Chief Developer: Akshar Miyani (MCA 1st Semester)                       ║
  ╠══════════════════════════════════════════════════════════════════════════╣
  ║  TAX INVOICE / ELECTRICITY BILL         Bill No: BILL-1001               ║
  ║  Billing Cycle: SEP-2026      Bill Date: 2026-09-12  Due Date: 2026-09-27║
  ╠══════════════════════════════════════════════════════════════════════════╣
  ║  CONSUMER DETAILS:                                                       ║
  ║  ID     : VB-1001       Name    : Dr. Rajesh Sharma                      ║
  ║  Meter  : MTR-DOM-8812  Category: Domestic (Residential)                 ║
  ║  Load   : 3.00 kW       Supply  : Single Phase (230V)                    ║
  ╠══════════════════════════════════════════════════════════════════════════╣
  ║  METER CONSUMPTION SUMMARY:                                              ║
  ║  Previous Index :     1610.0 kWh   Gross Consumed  :      380.0 kWh      ║
  ║  Current Index  :     1990.0 kWh   Solar Exported  :      180.0 kWh      ║
  ║  Power Factor   :       1.00       NET BILLED UNITS:      200.0 kWh      ║
  ╠══════════════════════════════════════════════════════════════════════════╣
  ║  SLAB-BASED ENERGY CHARGES BREAKDOWN:                                    ║
  ║  Tier 1      50.0 kWh           @ ₹   3.15      ₹         157.50         ║
  ║  Tier 2     100.0 kWh           @ ₹   4.50      ₹         450.00         ║
  ║  Tier 3      50.0 kWh           @ ₹   6.20      ₹         310.00         ║
  ║  Subtotal Energy Charges:                       ₹         917.50         ║
  ╠══════════════════════════════════════════════════════════════════════════╣
  ║  FIXED CHARGES & REGULATORY LEVIES:                                      ║
  ║  Fixed Sanctioned Load Charges               :  ₹         150.00         ║
  ║  Meter Hire & Instrument Rent                :  ₹          25.00         ║
  ║  Regulatory Asset Surcharge                  :  ₹          34.87         ║
  ║  State Electricity Duty (Govt. Tax)          :  ₹          53.38         ║
  ║  Clean Energy & Environment Cess             :  ₹          20.00         ║
  ║  Fuel Surcharge Adjustment (FPPCA)           :  ₹          70.00         ║
  ╠══════════════════════════════════════════════════════════════════════════╣
  ║    NET PAYABLE AMOUNT (BY DUE DATE)          :  ₹        1270.75         ║
  ╠══════════════════════════════════════════════════════════════════════════╣
  ║  Early Payment (If paid before 2026-09-27)   : Pay ₹ 1251.69             ║
  ║  Late Surcharge (If paid after 2026-09-27)    : Pay ₹ 1296.16             ║
  ╚══════════════════════════════════════════════════════════════════════════╝
```

---

## 🏛️ Project Directory Structure

```
VoltBill/
├── CMakeLists.txt              # Cross-platform CMake build & CPack packaging
├── Makefile                    # Standard Makefile for Linux/macOS/MinGW
├── build.bat                   # 1-click Windows CMD build script
├── build.ps1                   # 1-click Windows PowerShell build script
├── install.ps1                 # Windows automated web installer
├── install.sh                  # Linux/macOS automated web installer
├── LICENSE                     # MIT License (Akshar Miyani)
├── README.md                   # Complete documentation
├── .github/
│   └── workflows/
│       ├── build.yml           # Cross-platform CI verification
│       └── release.yml         # Automated multi-platform release packager
├── packaging/
│   ├── scoop/voltbill.json     # Scoop manifest
│   ├── brew/voltbill.rb        # Homebrew formula
│   └── winget/                 # Microsoft Winget package manifests
├── config/
│   └── tariffs.cfg             # Default runtime tariff configuration
├── data/                       # Binary data files & CSV exports
└── src/
    ├── main.c                  # Application entry point, CLI parser, event loop
    ├── models.h                # Core structs (Consumer, Bill, Tariff, Reading)
    ├── banner.h / banner.c     # ASCII artwork, onboarding & student identity
    ├── ui.h / ui.c             # ANSI color system, box-drawing & arrow menus
    ├── utils.h / utils.c       # Terminal UTF-8 raw mode, safe input, currency
    ├── tariff.h / tariff.c     # Slab engine, tariff parser & runtime editor
    ├── customer.h / customer.c # Customer registry, search, edit & profile cards
    ├── billing.h / billing.c   # Meter reading, progressive slab engine, invoice
    ├── payment.h / payment.c   # Payment simulation, receipts & transaction ledger
    ├── analytics.h / analytics.c # Unicode sparklines, grid metrics, carbon footprint
    └── storage.h / storage.c   # Binary persistence, CSV export & demo data seeder
```

---

## 👨‍💻 Developer & Institutional Attribution

* **Lead Architect:** **Akshar Miyani**
* **Academic Program:** Master of Computer Applications (MCA) — 1st Semester
* **Institution:** Manipal University Jaipur (MUJ), Rajasthan, India
* **GitHub:** [@miyaniakshar1234](https://github.com/miyaniakshar1234)
* **Project Repository:** [https://github.com/miyaniakshar1234/VoltBill](https://github.com/miyaniakshar1234/VoltBill)

---

<div align="center">
⚡ <i>Engineered with passion for systems programming & academic excellence at Manipal University Jaipur.</i>
</div>
