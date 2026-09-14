$ErrorActionPreference = 'Stop'
$packageName = 'voltbill'
$toolsDir = "$(Split-Path -parent $MyInvocation.MyCommand.Definition)"
$url = 'https://github.com/miyaniakshar1234/VoltBill/releases/download/v2.1.0/voltbill-windows-x64.zip'
$checksum = 'C275B1C97F1E8BD55C2436C56837A7F64794ECADB2EEE64DAB831B289D418B8C'
$checksumType = 'sha256'

Install-ChocolateyZipPackage -PackageName $packageName `
                            -Url $url `
                            -UnzipLocation $toolsDir `
                            -Checksum $checksum `
                            -ChecksumType $checksumType
