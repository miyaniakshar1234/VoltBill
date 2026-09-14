$ErrorActionPreference = 'Stop'
$packageName = 'voltbill'
$toolsDir = "$(Split-Path -parent $MyInvocation.MyCommand.Definition)"

Uninstall-ChocolateyZipPackage -PackageName $packageName `
                              -ZipFileName "$toolsDir\voltbill.exe"
