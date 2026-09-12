# VoltBill Native C Build Script for PowerShell
# Lead Architect: Akshar Miyani

Write-Host "=========================================================================" -ForegroundColor Cyan
Write-Host "  ⚡ VoltBill Native C Build System (PowerShell)" -ForegroundColor White
Write-Host "  Lead Architect: Akshar Miyani" -ForegroundColor Gray
Write-Host "=========================================================================`n" -ForegroundColor Cyan

Get-Process voltbill -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue
Start-Sleep -Milliseconds 200

New-Item -ItemType Directory -Force -Path "bin", "data", "config" | Out-Null

$vcvars = "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

if (Get-Command "cl.exe" -ErrorAction SilentlyContinue) {
    Write-Host "[FOUND] MSVC C Compiler (cl.exe)" -ForegroundColor Green
    & cl /nologo /O2 /W3 /D_CRT_SECURE_NO_WARNINGS /Fe:bin\voltbill.exe src\main.c src\utils.c src\ui.c src\banner.c src\tariff.c src\customer.c src\billing.c src\payment.c src\analytics.c src\storage.c src\qrcodegen.c
} elseif (Test-Path $vcvars) {
    Write-Host "[INFO] Activating MSVC BuildTools Environment..." -ForegroundColor Yellow
    cmd /c "`"$vcvars`" >nul && cl /nologo /O2 /W3 /D_CRT_SECURE_NO_WARNINGS /Fe:bin\voltbill.exe src\main.c src\utils.c src\ui.c src\banner.c src\tariff.c src\customer.c src\billing.c src\payment.c src\analytics.c src\storage.c src\qrcodegen.c"
} elseif (Get-Command "gcc" -ErrorAction SilentlyContinue) {
    Write-Host "[FOUND] GCC Compiler (gcc.exe)" -ForegroundColor Green
    & gcc -O2 -Wall -std=c99 -o bin/voltbill.exe src/main.c src/utils.c src/ui.c src/banner.c src/tariff.c src/customer.c src/billing.c src/payment.c src/analytics.c src/storage.c src/qrcodegen.c -lm
} elseif (Get-Command "clang" -ErrorAction SilentlyContinue) {
    Write-Host "[FOUND] Clang Compiler (clang.exe)" -ForegroundColor Green
    & clang -O2 -Wall -std=c99 -o bin/voltbill.exe src/main.c src/utils.c src/ui.c src/banner.c src/tariff.c src/customer.c src/billing.c src/payment.c src/analytics.c src/storage.c src/qrcodegen.c -lm
} else {
    Write-Host "[ERROR] No C compiler found (MSVC, GCC, or Clang)." -ForegroundColor Red
    exit 1
}

if (Test-Path "main.obj") { Remove-Item "*.obj" -Force -ErrorAction SilentlyContinue }

if (Test-Path "bin\voltbill.exe") {
    Write-Host "`n=========================================================================" -ForegroundColor Green
    Write-Host "  BUILD SUCCESSFUL!" -ForegroundColor Green
    Write-Host "  Executable generated at: bin\voltbill.exe" -ForegroundColor White
    Write-Host "=========================================================================" -ForegroundColor Green
} else {
    Write-Host "[ERROR] Build failed! voltbill.exe was not created." -ForegroundColor Red
    exit 1
}
