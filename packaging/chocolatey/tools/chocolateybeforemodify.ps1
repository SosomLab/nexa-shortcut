# 업그레이드/제거 전에 실행 중인 프로세스 종료
Get-Process -Name 'nShiftSpace*' -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue
