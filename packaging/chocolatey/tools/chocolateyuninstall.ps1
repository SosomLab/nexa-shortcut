$ErrorActionPreference = 'Stop'
$toolsDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$exePattern = 'nShiftSpace-x*.exe'

# 시작 메뉴 바로가기 제거
$shortcut = Join-Path ([Environment]::GetFolderPath('CommonPrograms')) 'nShiftSpace.lnk'
if (Test-Path $shortcut) { Remove-Item $shortcut -Force -ErrorAction SilentlyContinue }

# 자동 실행 값이 이 패키지의 exe(또는 0.3.0 이하의 shim)를 가리킬 때만 지운다.
# 다른 곳에 둔 사본이나 다른 판(일반/LED)을 가리키는 값은 그대로 둔다.
try {
  $runKey = 'HKCU:\Software\Microsoft\Windows\CurrentVersion\Run'
  $cur = (Get-ItemProperty -Path $runKey -Name 'nShiftSpace' -ErrorAction SilentlyContinue).nShiftSpace
  if ($cur) {
    $path = $cur.Trim('"')
    $dir = Split-Path $path -Parent
    $binDir = Join-Path $env:ChocolateyInstall 'bin'
    if (($dir -ieq $toolsDir -or $dir -ieq $binDir) -and (Split-Path $path -Leaf) -like $exePattern) {
      Remove-ItemProperty -Path $runKey -Name 'nShiftSpace' -ErrorAction SilentlyContinue
      Write-Host "Removed the 'Start at login' entry."
    }
  }
} catch {
  Write-Warning "Could not check the 'Start at login' entry: $($_.Exception.Message)"
}
