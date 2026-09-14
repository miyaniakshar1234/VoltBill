# VoltBill One-Command Windows Installer
# Lead Systems Architect: Akshar Miyani
# Usage: irm https://raw.githubusercontent.com/miyaniakshar1234/VoltBill/main/install.ps1 | iex

$ErrorActionPreference = "Stop"

Write-Host ""
Write-Host "  =========================================================================" -ForegroundColor Cyan
Write-Host "    * Installing VoltBill Utility Engine (Windows x64)" -ForegroundColor White
Write-Host "    Lead Systems Architect: Akshar Miyani" -ForegroundColor Gray
Write-Host "  =========================================================================" -ForegroundColor Cyan
Write-Host ""

$BaseDir = "$env:LOCALAPPDATA\VoltBill"
$InstallDir = Join-Path $BaseDir "bin"
$ConfigDir = Join-Path $BaseDir "config"
$ExePath = Join-Path $InstallDir "voltbill.exe"

New-Item -ItemType Directory -Force -Path $InstallDir, $ConfigDir | Out-Null

$Repo = "miyaniakshar1234/VoltBill"
$ReleaseUrl = "https://github.com/$Repo/releases/latest/download/voltbill-windows-x64.zip"

$LocalExe = $null
if ($PSScriptRoot -and (Test-Path (Join-Path $PSScriptRoot "bin\voltbill.exe"))) {
    $LocalExe = Join-Path $PSScriptRoot "bin\voltbill.exe"
} elseif (Test-Path ".\bin\voltbill.exe") {
    $LocalExe = (Resolve-Path ".\bin\voltbill.exe").Path
}

if ($LocalExe) {
    Write-Host "  [1/3] Installing local binary ($LocalExe)..." -ForegroundColor Yellow
    Copy-Item $LocalExe -Destination $ExePath -Force
    if (Test-Path "config\tariffs.cfg") {
        Copy-Item "config\tariffs.cfg" -Destination (Join-Path $ConfigDir "tariffs.cfg") -Force
    }
} else {
    Write-Host "  [1/3] Downloading latest release from GitHub ($ReleaseUrl)..." -ForegroundColor Yellow
    $ZipPath = Join-Path $env:TEMP "voltbill-windows-x64.zip"
    $TempExtract = Join-Path $env:TEMP "voltbill_extract"
    try {
        [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12 -bor [Net.SecurityProtocolType]::Tls13
        Invoke-WebRequest -Uri $ReleaseUrl -OutFile $ZipPath -UseBasicParsing
        if (Test-Path $TempExtract) { Remove-Item $TempExtract -Recurse -Force -ErrorAction SilentlyContinue }
        Expand-Archive -Path $ZipPath -DestinationPath $TempExtract -Force
        if (Test-Path "$TempExtract\voltbill.exe") {
            Copy-Item "$TempExtract\voltbill.exe" -Destination $ExePath -Force
        } elseif (Test-Path "$TempExtract\bin\voltbill.exe") {
            Copy-Item "$TempExtract\bin\voltbill.exe" -Destination $ExePath -Force
        }
        if (Test-Path "$TempExtract\tariffs.cfg") {
            Copy-Item "$TempExtract\tariffs.cfg" -Destination (Join-Path $ConfigDir "tariffs.cfg") -Force
        } elseif (Test-Path "$TempExtract\config\tariffs.cfg") {
            Copy-Item "$TempExtract\config\tariffs.cfg" -Destination (Join-Path $ConfigDir "tariffs.cfg") -Force
        }
        Remove-Item $ZipPath -Force -ErrorAction SilentlyContinue
        Remove-Item $TempExtract -Recurse -Force -ErrorAction SilentlyContinue
    } catch {
        Write-Host "  [!] Release binary download failed. Attempting local compilation..." -ForegroundColor Yellow
        if (Test-Path "build.ps1") {
            powershell -ExecutionPolicy Bypass -File .\build.ps1
            if (Test-Path "bin\voltbill.exe") {
                Copy-Item "bin\voltbill.exe" -Destination $ExePath -Force
            }
        } else {
            Write-Host "  [ERROR] Could not download or build voltbill.exe." -ForegroundColor Red
            exit 1
        }
    }
}

Write-Host "  [2/3] Configuring Environment PATH..." -ForegroundColor Yellow
$UserPath = [Environment]::GetEnvironmentVariable("Path", "User")
if ($UserPath -notlike "*$InstallDir*") {
    $NewPath = if ($UserPath.EndsWith(";")) { "$UserPath$InstallDir" } else { "$UserPath;$InstallDir" }
    [Environment]::SetEnvironmentVariable("Path", $NewPath, "User")
    $env:Path = "$env:Path;$InstallDir"
    Write-Host "  [+] Added $InstallDir to User PATH." -ForegroundColor Green
} else {
    Write-Host "  [*] $InstallDir is already present in PATH." -ForegroundColor Gray
}

Write-Host "  [3/3] Verifying Installation..." -ForegroundColor Yellow
if (Test-Path $ExePath) {
    Write-Host ""
    Write-Host "  =========================================================================" -ForegroundColor Green
    Write-Host "    SUCCESS: VoltBill is installed and ready to use." -ForegroundColor Green
    Write-Host "    Location: $ExePath" -ForegroundColor Gray
    Write-Host ""
    Write-Host "    You can now run in any terminal:" -ForegroundColor White
    Write-Host "        voltbill" -ForegroundColor Cyan
    Write-Host "        voltbill --demo" -ForegroundColor Cyan
    Write-Host "        voltbill scada" -ForegroundColor Cyan
    Write-Host "  =========================================================================" -ForegroundColor Green
    Write-Host ""
} else {
    Write-Host "  [ERROR] Installation verification failed: $ExePath not found." -ForegroundColor Red
    exit 1
}
