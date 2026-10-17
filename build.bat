@echo off
rem nShiftSpace — Windows 네이티브 빌드 (Visual Studio / MSVC)
rem 시작 메뉴의 "x64 Native Tools Command Prompt for VS"에서 실행하세요.
rem 초경량 원칙: CRT 미링크 (/NODEFAULTLIB + /ENTRY:start), 크기 최적화 /O1
rem 결과물: dist\nShiftSpace-x64.exe (일반), dist\nShiftSpace-led-x64.exe (키보드 LED 표시판)
setlocal

where cl >nul 2>nul
if errorlevel 1 (
    echo [오류] cl.exe 를 찾을 수 없습니다.
    echo        "x64 Native Tools Command Prompt for VS" 에서 실행하세요.
    exit /b 1
)

if not exist build mkdir build
if not exist dist  mkdir dist

rem ---- 일반판 ----
rc /nologo /fo build\nShiftSpace.res res\nShiftSpace.rc
if errorlevel 1 exit /b 1

cl /nologo /utf-8 /O1 /GS- /DUNICODE /D_UNICODE ^
   src\nShiftSpace.c build\nShiftSpace.res ^
   /Fo:build\ /Fe:dist\nShiftSpace-x64.exe ^
   /link /SUBSYSTEM:WINDOWS /ENTRY:start /NODEFAULTLIB ^
   kernel32.lib user32.lib shell32.lib advapi32.lib imm32.lib
if errorlevel 1 exit /b 1

rem ---- LED판 (NSS_LED: 켜짐 아이콘 리소스 + Raw Input 코드 포함) ----
rc /nologo /d NSS_LED /fo build\nShiftSpace-led.res res\nShiftSpace.rc
if errorlevel 1 exit /b 1

cl /nologo /utf-8 /O1 /GS- /DUNICODE /D_UNICODE /DNSS_LED ^
   src\nShiftSpace.c build\nShiftSpace-led.res ^
   /Fo:build\ /Fe:dist\nShiftSpace-led-x64.exe ^
   /link /SUBSYSTEM:WINDOWS /ENTRY:start /NODEFAULTLIB ^
   kernel32.lib user32.lib shell32.lib advapi32.lib imm32.lib
if errorlevel 1 exit /b 1

echo.
echo 빌드 완료: dist\nShiftSpace-x64.exe, dist\nShiftSpace-led-x64.exe
