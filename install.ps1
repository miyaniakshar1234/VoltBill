# VoltBill One-Command Windows Installer
# Lead Architect: Akshar Miyani (MCA 1st Sem, MUJ)
# Usage: irm https://raw.githubusercontent.com/miyaniakshar1234/VoltBill/main/install.ps1 | iex

$ErrorActionPreference = "Stop"

Write-Host "`n  =========================================================================" -ForegroundColor Cyan
Write-Host "    ⚡ Installing VoltBill Utility Engine (Windows x64)" -ForegroundColor White
Write-Host "    Lead Architect: Akshar Miyani | MCA (1st Sem) | Manipal University Jaipur" -ForegroundColor Gray
Write-Host "  =========================================================================`n" -ForegroundColor Cyan

$InstallDir = "$env:LOCALAPPDATA\VoltBill\bin"
$ExePath = Join-Path $InstallDir "voltbill.exe"

New-Item -ItemType Directory -Force -Path $InstallDir | Out-Null

$Repo = "miyaniakshar1234/VoltBill"
$ReleaseUrl = "https://github.com/$Repo/releases/latest/download/voltbill-windows-x64.zip"

# Check if compiling locally from repository clone or downloading release
$LocalExe = Join-Path $PSScriptRoot "bin\voltbill.exe"
if (Test-Path $LocalExe) {
    Write-Host "  [1/3] Copying local binary to $InstallDir..." -ForegroundColor Yellow
    Copy-Item $LocalExe -Destination $ExePath -Force
} else {
    Write-Host "  [1/3] Downloading latest release from GitHub ($ReleaseUrl)..." -ForegroundColor Yellow
    $ZipPath = Join-Path $env:TEMP "voltbill-windows-x64.zip"
    try {
        Invoke-WebRequest -Uri $ReleaseUrl -OutFile $ZipPath -UseBasicParsing
        Expand-Archive -Path $ZipPath -DestinationPath $InstallDir -Force
        Remove-Item $ZipPath -Force -ErrorAction SilentlyContinue
    } catch {
        Write-Host "  [INFO] Release binary not found online yet. Compiling locally..." -ForegroundColor Cyan
        if (Test-Path "build.ps1") {
            & .\build.ps1
            Copy-Item "bin\voltbill.exe" -Destination $ExePath -Force
        } else {
            Write-Host "  [ERROR] Unable to download or compile voltbill." -ForegroundColor Red
            exit 1
        }
    }
}

# Add to user PATH if not present
Write-Host "  [2/3] Configuring Environment PATH..." -ForegroundColor Yellow
$UserPath = [Environment]::GetEnvironmentVariable("Path", "User")
if ($UserPath -notlike "*$InstallDir*") {
    [Environment]::SetEnvironmentVariable("Path", "$UserPath;$InstallDir", "User")
    $env:Path += ";$InstallDir"
    Write-Host "  [+] Added $InstallDir to User PATH." -ForegroundColor Green
} else {
    Write-Host "  [*] $InstallDir is already present in PATH." -ForegroundColor Gray
}

Write-Host "  [3/3] Verifying Installation..." -ForegroundColor Yellow
if (Test-Path $ExePath) {
    Write-Host "`n  =========================================================================" -ForegroundColor Green
    Write-Host "    ✓ SUCCESS! VoltBill is installed and ready to use." -ForegroundColor Green
    Write-Host "    You can now open any terminal and run:" -ForegroundColor White
    Write-Host "        voltbill" -ForegroundColor Cyan
    Write-Host "        voltbill --demo" -ForegroundColor Cyan
    Write-Host "  =========================================================================`n" -ForegroundColor Green
} else {
    Write-Host "  [ERROR] Installation verification failed." -ForegroundColor Red
}
