# 업그레이드/제거 전에 실행 중인 프로세스 종료 (LED판만)
Get-Process -Name 'nShiftSpace-led*' -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue
