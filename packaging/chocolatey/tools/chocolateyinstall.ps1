$ErrorActionPreference = 'Stop'
$toolsDir = Split-Path -Parent $MyInvocation.MyCommand.Definition

$packageArgs = @{
  packageName    = 'nshiftspace'
  unzipLocation  = $toolsDir
  url            = 'https://github.com/SosomLab/nexa-shortcut/releases/download/v__VERSION__/nShiftSpace-x86.zip'
  url64bit       = 'https://github.com/SosomLab/nexa-shortcut/releases/download/v__VERSION__/nShiftSpace-x64.zip'
  checksum       = '__CHECKSUM32__'
  checksumType   = 'sha256'
  checksum64     = '__CHECKSUM64__'
  checksumType64 = 'sha256'
}
Install-ChocolateyZipPackage @packageArgs

$exePattern = 'nShiftSpace-x*.exe'
$exe = Get-ChildItem $toolsDir -Filter $exePattern | Select-Object -First 1

# shim을 만들지 않는다(.ignore). ShimGen shim은 콘솔 프로그램이라, 자동 실행이나 탐색기에서
# 실행하면 Windows가 콘솔 창을 잠깐 띄운다(.gui 마커로는 막지 못한다). 대신 시작 메뉴 바로가기를 둔다.
Get-ChildItem $toolsDir -Filter $exePattern | ForEach-Object {
  New-Item "$($_.FullName).ignore" -ItemType File -Force | Out-Null
}

$shortcut = Join-Path ([Environment]::GetFolderPath('CommonPrograms')) 'nShiftSpace.lnk'
try {
  Install-ChocolateyShortcut -ShortcutFilePath $shortcut -TargetPath $exe.FullName `
    -Description 'nShiftSpace - Shift+Space IME toggle'
} catch {
  Write-Warning "Start menu shortcut was not created: $($_.Exception.Message)"
}

# 0.3.0 이하에서 자동 실행 값이 shim(chocolatey\bin)을 가리키고 있으면 실제 exe 경로로 옮긴다.
# shim이 없어지므로 그대로 두면 자동 실행이 끊긴다. 다른 경로를 가리키는 값은 건드리지 않는다.
try {
  $runKey = 'HKCU:\Software\Microsoft\Windows\CurrentVersion\Run'
  $cur = (Get-ItemProperty -Path $runKey -Name 'nShiftSpace' -ErrorAction SilentlyContinue).nShiftSpace
  if ($cur) {
    $path = $cur.Trim('"')
    $binDir = Join-Path $env:ChocolateyInstall 'bin'
    if ((Split-Path $path -Parent) -ieq $binDir -and (Split-Path $path -Leaf) -like $exePattern) {
      Set-ItemProperty -Path $runKey -Name 'nShiftSpace' -Value ('"{0}"' -f $exe.FullName)
      Write-Host "Updated the 'Start at login' entry to point at $($exe.FullName)."
    }
  }
} catch {
  Write-Warning "Could not check the 'Start at login' entry: $($_.Exception.Message)"
}

Write-Host "nShiftSpace installed. Start it from the Start menu ('nShiftSpace') or run $($exe.FullName)" -ForegroundColor Green
Write-Host "To run at login: tray icon > 'Start at login'." -ForegroundColor Green
