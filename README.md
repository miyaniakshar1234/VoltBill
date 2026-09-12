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
### High-Performance Console Electricity Billing & Grid Utility Engine in ISO C

[![C Standard](https://img.shields.io/badge/C-C99%20%2F%20C11-00F0FF.svg?style=for-the-badge&logo=c)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-BD00FF.svg?style=for-the-badge)](https://github.com/miyaniakshar1234/VoltBill)
[![License: MIT](https://img.shields.io/badge/License-MIT-00FFA3.svg?style=for-the-badge)](LICENSE)
[![Build Status](https://img.shields.io/badge/CI-Passing-FFE600.svg?style=for-the-badge)](https://github.com/miyaniakshar1234/VoltBill/actions)

**Lead Architect & Developer:** **Akshar Miyani** ([@miyaniakshar1234](https://github.com/miyaniakshar1234))  
**Language Standards:** ISO C99 / ISO C11 Native Core  

---

</div>

## 📌 Executive Summary

**VoltBill** is an enterprise-grade utility distribution engine and customer management system written in native ISO C. Featuring a **cyber-aesthetic ANSI terminal user interface**, **multi-tier progressive slab billing**, **solar rooftop net-metering**, **industrial power factor regulation**, **Time-of-Day (ToD) peak-hour tariffs**, **defaulter notice automation**, **grid-wide batch billing**, and **scriptable CLI subcommands**, VoltBill is engineered for maximum throughput, low memory overhead, and cross-platform native execution.

---

## ✨ Powerhouse Feature Matrix

| Feature Domain | Capabilities & Systems Architecture |
| :--- | :--- |
| **👤 Customer Management** | Unique alphanumeric Consumer IDs (`VB-1001`), meter serial tracking, contact metadata, connection categorization (Domestic, Commercial, Industrial, Agricultural), phase selection (Single/3-Phase), and contract demand in kW. |
| **⚡ Meter Reading & Validation** | Previous & current reading integrity checks, meter dial rollover recovery, defective meter flagging, and consumption anomaly alerts. |
| **☀️ Green Solar Net-Metering** | Deducts solar energy exported back to the grid from gross grid consumption, automatically computing net billed units and environmental offsets. |
| **📈 Progressive Slab Engine** | Multi-tier slab billing across categories (e.g. Domestic: 0–50 @ ₹3.15, 51–150 @ ₹4.50, 151–300 @ ₹6.20, >300 @ ₹7.95). |
| **⏰ Time-of-Day (ToD) Tariffs** | Enforces peak-hour surcharges (18:00 - 22:00) and solar daylight off-peak rebates for commercial and industrial loads. |
| **🏭 Industrial Power Factor** | Enforces lagging power factor penalty surcharges (PF < 0.90) and incentives (PF > 0.95) to maintain grid stability. |
| **🧾 Tax & Surcharge Levies** | Computes Sanctioned Load Demand Charges, Meter Rent, Regulatory Asset Surcharge, State Electricity Duty, Green Energy Cess, and Fuel Surcharge (FPPCA). |
| **🚀 Batch Grid Billing Run** | Executes automated grid-wide billing cycles across all active consumers in seconds, rendering real-time terminal progress bars (`ui_progress_bar`). |
| **⚠️ Defaulters & Notices** | Identifies delinquent accounts with outstanding arrears and generates official text **Power Disconnection Notices** (`data/notices/NOTICE_VB-xxxx.txt`). |
| **📊 Visual Terminal Analytics** | Renders horizontal energy consumption bar charts using UTF-8 blocks (` ▂▃▄▅▆▇█`), daily burn rates, and predictive projections. |
| **🌱 Carbon Footprint Engine** | Computes greenhouse emissions ($0.82 \text{ kg } CO_2\text{/kWh}$) and calculates mature trees required to neutralize the footprint. |
| **💳 Payment & Audit Ledger** | Supports Cash, UPI, Credit/Debit Cards, and NetBanking. Generates verified receipts (`REC-xxxx`), tracks arrears, and advances. |
| **🔒 Security Audit Trail** | Appends timestamped actions to `data/audit_trail.log` recording customer additions, billing events, and payments. |
| **⚙️ Dynamic Tariff Config** | Runtime editable tariffs stored in `config/tariffs.cfg` — modify slab rates and taxes without recompiling code! |
| **💾 Persistence & JSON Backup** | Fast binary datastores (`data/*.dat`), automated CSV spreadsheets (`data/export_*.csv`), and timestamped JSON snapshots (`data/backups/`). |
| **💻 Scriptable CLI Subcommands** | Direct command-line automation (`voltbill bill`, `voltbill pay`, `voltbill status`, `voltbill batch`, `voltbill backup`). |

---

## 🚀 Quick Installation & Distribution

VoltBill is cross-platform and deployable across Windows, Linux, and macOS:

### 🪟 Windows (PowerShell One-Liner)
```powershell
irm https://raw.githubusercontent.com/miyaniakshar1234/VoltBill/main/install.ps1 | iex
```

### 🐧 Linux & 🍎 macOS (Shell One-Liner)
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

## 💻 CLI Subcommands & Scripting Mode

VoltBill can be operated interactively or automated directly from shell scripts:

```bash
# Launch interactive TUI dashboard
voltbill

# 🌟 Seed realistic demo records & launch dashboard
voltbill --demo

# ⚡ Run grid-wide batch billing cycle for all active consumers
voltbill --batch

# 💾 Create full timestamped JSON backup snapshot
voltbill --backup

# 📊 Export all database records to CSV spreadsheets and exit
voltbill --export

# 📋 Generate an invoice directly from command line
voltbill bill VB-1001 2150.0

# 💳 Record a payment directly from command line (mode: 0=Cash, 1=UPI, 2=Card, 3=NetBank)
voltbill pay BILL-1001 1250.00 1

# 🔍 Query instant consumer ledger balance and meter status
voltbill status VB-1001

# Display developer info and version
voltbill --version
```

---

## 🖥️ Terminal UI Preview

### 1. Main Control Dashboard
```
  VOLTBILL v1.1.0  │  MAIN CONTROL DASHBOARD
  Use Arrow Keys [↑/↓] or [1-9] to navigate, [Enter] to select, [Esc] to return
  ────────────────────────────────────────────────────────────────────────────

  ┌────────────────────────────────────────────────────────────────────────┐
  │   ► [1] Customer Registration & Profiles                               │
  │     [2] Meter Reading & Bill Generation                                │
  │     [3] Payment Processing & Receipts                                  │
  │     [4] Grid Analytics & Carbon Footprint                              │
  │     [5] Dynamic Tariff & Slab Rules                                    │
  │     [6] System Tools, Seeder & CSV Export                              │
  │     [7] About Developer & Architecture                                 │
  │     [8] Save & Exit VoltBill Engine                                    │
  └────────────────────────────────────────────────────────────────────────┘
```

### 2. Itemized Final Bill & Tax Invoice
```
  ╔══════════════════════════════════════════════════════════════════════════╗
  ║  VOLTBILL STATE UTILITY DISTRIBUTION COMPANY                             ║
  ║  High-Performance Systems Engineering Engine                             ║
  ║  Lead Architect: Akshar Miyani                                           ║
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

## 🏛️ Project Architecture & Layout

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
├── data/                       # Binary data files, CSV exports, notices & backups
└── src/
    ├── main.c                  # Application entry point, CLI parser, event loop
    ├── models.h                # Core structs (Consumer, Bill, Tariff, Reading)
    ├── banner.h / banner.c     # ASCII artwork, onboarding & developer identity
    ├── ui.h / ui.c             # ANSI color system, box-drawing & arrow menus
    ├── utils.h / utils.c       # Terminal UTF-8 raw mode, safe input, audit log
    ├── tariff.h / tariff.c     # Slab engine, tariff parser & runtime editor
    ├── customer.h / customer.c # Customer registry, search, edit & defaulters
    ├── billing.h / billing.c   # Meter reading, progressive slab engine, batch billing
    ├── payment.h / payment.c   # Payment simulation, receipts & transaction ledger
    ├── analytics.h / analytics.c # Unicode sparklines, grid metrics, carbon footprint
    └── storage.h / storage.c   # Binary persistence, CSV export & JSON backup
```

---

## 👨‍💻 Lead Architect

* **Developer:** **Akshar Miyani**
* **GitHub Profile:** [@miyaniakshar1234](https://github.com/miyaniakshar1234)
* **Source Repository:** [https://github.com/miyaniakshar1234/VoltBill](https://github.com/miyaniakshar1234/VoltBill)

---

<div align="center">
⚡ <i>High-performance native systems programming by Akshar Miyani.</i>
</div>
