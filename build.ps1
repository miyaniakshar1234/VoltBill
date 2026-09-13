param(
    [switch]$Test
)

Write-Host "=========================================================================" -ForegroundColor Cyan
Write-Host "  ⚡ VoltBill Native C Build System (PowerShell)" -ForegroundColor White
Write-Host "  Lead Architect: Akshar Miyani" -ForegroundColor Gray
Write-Host "=========================================================================`n" -ForegroundColor Cyan

Get-Process voltbill -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue
Start-Sleep -Milliseconds 200

New-Item -ItemType Directory -Force -Path "bin", "data", "config", "tests" | Out-Null

$vcvars = "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
$srcFiles = "src\main.c src\utils.c src\ui.c src\banner.c src\tariff.c src\customer.c src\billing.c src\payment.c src\analytics.c src\storage.c src\qrcodegen.c src\modbus.c"
$testSrcFiles = "tests\test_suite.c src\utils.c src\ui.c src\banner.c src\tariff.c src\customer.c src\billing.c src\payment.c src\analytics.c src\storage.c src\qrcodegen.c src\modbus.c"

if (Get-Command "cl.exe" -ErrorAction SilentlyContinue) {
    Write-Host "[FOUND] MSVC C Compiler (cl.exe)" -ForegroundColor Green
    & cl /nologo /O2 /W3 /D_CRT_SECURE_NO_WARNINGS /Fe:bin\voltbill.exe ($srcFiles -split ' ')
    if ($Test) {
        & cl /nologo /O2 /W3 /D_CRT_SECURE_NO_WARNINGS /Fe:bin\test_suite.exe ($testSrcFiles -split ' ')
    }
} elseif (Test-Path $vcvars) {
    Write-Host "[INFO] Activating MSVC BuildTools Environment..." -ForegroundColor Yellow
    cmd /c "`"$vcvars`" >nul && cl /nologo /O2 /W3 /D_CRT_SECURE_NO_WARNINGS /Fe:bin\voltbill.exe $srcFiles"
    if ($Test) {
        cmd /c "`"$vcvars`" >nul && cl /nologo /O2 /W3 /D_CRT_SECURE_NO_WARNINGS /Fe:bin\test_suite.exe $testSrcFiles"
    }
} elseif (Get-Command "gcc" -ErrorAction SilentlyContinue) {
    Write-Host "[FOUND] GCC Compiler (gcc.exe)" -ForegroundColor Green
    & gcc -O2 -Wall -std=c99 -o bin/voltbill.exe ($srcFiles -split ' ') -lm
    if ($Test) {
        & gcc -O2 -Wall -std=c99 -o bin/test_suite.exe ($testSrcFiles -split ' ') -lm
    }
} elseif (Get-Command "clang" -ErrorAction SilentlyContinue) {
    Write-Host "[FOUND] Clang Compiler (clang.exe)" -ForegroundColor Green
    & clang -O2 -Wall -std=c99 -o bin/voltbill.exe ($srcFiles -split ' ') -lm
    if ($Test) {
        & clang -O2 -Wall -std=c99 -o bin/test_suite.exe ($testSrcFiles -split ' ') -lm
    }
} else {
    Write-Host "[ERROR] No C compiler found (MSVC, GCC, or Clang)." -ForegroundColor Red
    exit 1
}

if (Test-Path "*.obj") { Remove-Item "*.obj" -Force -ErrorAction SilentlyContinue }

if (Test-Path "bin\voltbill.exe") {
    Write-Host "`n=========================================================================" -ForegroundColor Green
    Write-Host "  BUILD SUCCESSFUL!" -ForegroundColor Green
    Write-Host "  Executable generated at: bin\voltbill.exe" -ForegroundColor White
    Write-Host "=========================================================================" -ForegroundColor Green
} else {
    Write-Host "[ERROR] Build failed! voltbill.exe was not created." -ForegroundColor Red
    exit 1
}

if ($Test) {
    if (Test-Path "bin\test_suite.exe") {
        Write-Host "`n[RUNNING TEST SUITE]..." -ForegroundColor Cyan
        & .\bin\test_suite.exe
        if ($LASTEXITCODE -ne 0) {
            exit $LASTEXITCODE
        }
    }
}
