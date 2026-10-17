$ErrorActionPreference = 'Stop'
$toolsDir = Split-Path -Parent $MyInvocation.MyCommand.Definition

$packageArgs = @{
  packageName    = 'nshiftspace-led'
  unzipLocation  = $toolsDir
  url            = 'https://github.com/SosomLab/nexa-shortcut/releases/download/v__VERSION__/nShiftSpace-led-x86.zip'
  url64bit       = 'https://github.com/SosomLab/nexa-shortcut/releases/download/v__VERSION__/nShiftSpace-led-x64.zip'
  checksum       = '__CHECKSUM32__'
  checksumType   = 'sha256'
  checksum64     = '__CHECKSUM64__'
  checksumType64 = 'sha256'
}
Install-ChocolateyZipPackage @packageArgs

# GUI 프로그램이므로 shim이 콘솔을 붙잡지 않도록 .gui 마커 생성
Get-ChildItem $toolsDir -Filter 'nShiftSpace-led*.exe' | ForEach-Object {
  New-Item "$($_.FullName).gui" -ItemType File -Force | Out-Null
}

Write-Host "nShiftSpace LED installed. Run 'nShiftSpace-led-x64' (or -x86) to start." -ForegroundColor Green
Write-Host "To run at login: tray icon > 'Start at login'." -ForegroundColor Green
