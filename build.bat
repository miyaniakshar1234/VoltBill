@echo off
setlocal enabledelayedexpansion

echo =========================================================================
echo   VoltBill Native C Build System
echo   Lead Architect: Akshar Miyani
echo =========================================================================
echo.

taskkill /F /IM voltbill.exe >nul 2>nul

if not exist bin mkdir bin
if not exist data mkdir data
if not exist config mkdir config

rem Check for cl
where cl >nul 2>nul
if %errorlevel% equ 0 (
    echo [FOUND] MSVC C Compiler (cl.exe)
    set COMPILER=MSVC
    goto DO_BUILD
)

rem Try locating vcvars64.bat from Visual Studio Build Tools
set "VCVARS=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
if exist "%VCVARS%" (
    echo [INFO] Activating MSVC BuildTools Environment
    call "%VCVARS%" >nul
    set COMPILER=MSVC
    goto DO_BUILD
)

rem Check for gcc
where gcc >nul 2>nul
if %errorlevel% equ 0 (
    echo [FOUND] GCC Compiler
    set COMPILER=GCC
    goto DO_BUILD
)

rem Check for clang
where clang >nul 2>nul
if %errorlevel% equ 0 (
    echo [FOUND] Clang Compiler
    set COMPILER=CLANG
    goto DO_BUILD
)

echo [ERROR] No C compiler detected.
exit /b 1

:DO_BUILD
if "%COMPILER%"=="MSVC" (
    echo [BUILD] Compiling with MSVC cl.exe...
    cl /nologo /O2 /W3 /D_CRT_SECURE_NO_WARNINGS /Fe:bin\voltbill.exe src\main.c src\utils.c src\ui.c src\banner.c src\tariff.c src\customer.c src\billing.c src\payment.c src\analytics.c src\storage.c src\qrcodegen.c
    if %errorlevel% neq 0 (
        echo [ERROR] Compilation failed!
        exit /b %errorlevel%
    )
    if exist *.obj del *.obj >nul 2>nul
) else (
    echo [BUILD] Compiling with %COMPILER%...
    gcc -O2 -Wall -std=c99 -o bin/voltbill.exe src/main.c src/utils.c src/ui.c src/banner.c src/tariff.c src/customer.c src/billing.c src/payment.c src/analytics.c src/storage.c src/qrcodegen.c -lm
    if %errorlevel% neq 0 (
        echo [ERROR] Compilation failed!
        exit /b %errorlevel%
    )
)

echo.
echo =========================================================================
echo   BUILD SUCCESSFUL!
echo   Executable generated at: bin\voltbill.exe
echo   Run with: bin\voltbill.exe
echo =========================================================================
