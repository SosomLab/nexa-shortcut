/*
 * nShiftSpace (Nexa ShiftSpace) — Shift+Space 한/영 전환 단일 기능 상주 유틸.
 *
 * 초경량 원칙: CRT를 링크하지 않는다 (-nostdlib, 진입점 start).
 *  - 큰 구조체는 전역(.bss)에 두어 memset 호출 생성을 피한다.
 *  - 메모리 표시(작업 관리자 "메모리" = 사설 작업 집합) 최소화 — 두 가지:
 *    1) ImmDisableIME(-1): 트레이 메뉴를 열 때 SetForegroundWindow 로 우리 창이 활성화되면
 *       한국어 IME/TSF 스택(msctf, imkrtip, imjkapi, TextInputFramework, CoreMessaging 등 DLL 20여 개와
 *       그 스레드)이 프로세스에 붙어 이후 포그라운드가 바뀔 때마다 페이지를 끌어들인다. 우리는
 *       텍스트 입력을 받지 않으므로 이 스레드의 IME 를 아예 끈다. (키 합성·단축키·메뉴와 무관)
 *    2) 마지막 메시지 후 2초 동안 조용하면 일회성 타이머로 SetProcessWorkingSetSize(-1/-1).
 *       user32/shell32 가 건드린 DLL 데이터 페이지(복사-후-쓰기 사설 페이지)를 대기 목록으로 내려
 *       수치가 수백 KB 로 떨어진다. 커밋 메모리 자체는 변하지 않는다 — 표시 수치와 상주 페이지만
 *       줄어들며, 다음 단축키 처리 때 필요한 몇 페이지만 소프트 폴트로 돌아온다. 주기 타이머가
 *       아니라 활동 뒤 한 번만 깨어난다 (유휴 CPU 0 유지).
 *  - 라이브러리는 kernel32 / user32 / shell32 만 사용한다.
 *    (예외 1: advapi32 — "로그인 시 자동 실행" 메뉴가 HKCU\...\Run 값을 읽고 쓰는 데만 사용.
 *     예외 2: imm32 — ImmDisableIME 하나. 둘 다 user32 가 이미 로드하는 DLL이라 추가 부담 없음.)
 *
 * 트레이 메뉴:
 *  - 맨 위에 프로그램 이름과 버전 (누를 수 없는 머리 줄)
 *  - 시작 메뉴에 바로가기 (체크) — 사용자 시작 메뉴(Programs)에 자기 exe를 가리키는 .lnk를 만들거나 지운다.
 *    .lnk 저장에는 COM(IShellLink)이 필요한데, ole32 를 import 하면 시작할 때부터 로드되어 메모리가 늘어난다.
 *    그래서 메뉴를 누른 순간에만 LoadLibrary 로 불러 쓰고 바로 내린다. Chocolatey 패키지가 공용 시작 메뉴에
 *    같은 이름의 바로가기를 이미 두었으면 체크된 채 비활성으로 보여 준다(공용 쪽은 관리자만 지울 수 있다).
 *  - 로그인 시 자동 실행 (체크) — HKCU\Software\Microsoft\Windows\CurrentVersion\Run 에
 *    "nShiftSpace" 값으로 자기 exe 경로(따옴표 포함)를 쓰거나 지운다. 사용자가 켤 때만 쓴다 (기본: 안 씀).
 *  - 키보드 LED 표시 (체크, NSS_LED 빌드만)
 *  - 종료
 *
 * 빌드 변형 (컴파일 플래그):
 *  - (기본)    키 입력을 전혀 감시하지 않는다. CPU 0.
 *  - -DNSS_LED 키보드 LED 표시 — 아무 키나 누르면 트레이 아이콘이 켜지고
 *              떼면 꺼진다. Raw Input(RIDEV_INPUTSINK)으로 키 make/break만
 *              수신하므로 훅(WH_KEYBOARD_LL)과 달리 다른 프로그램의 키 입력을
 *              지연시키지 않는다. 리소스에 켜짐 아이콘(IDI_TRAY_ON)이 추가된다.
 *              트레이 메뉴 "키보드 LED 표시"로 켜고 끌 수 있으며, 끄면 Raw Input
 *              등록을 해제(RIDEV_REMOVE)해 WM_INPUT이 전혀 오지 않는다 — 즉
 *              기본 빌드와 동일하게 키 입력당 비용 0. 설정은 저장하지 않는다
 *              (무설정 원칙): 시작 시 항상 켜짐.
 */
#include <windows.h>
#include <shlobj.h>   /* IShellLinkW · IPersistFile 선언만 쓴다 — ole32 는 import 하지 않는다 */

#ifndef MOD_NOREPEAT
#define MOD_NOREPEAT 0x4000  /* Vista 이전 SDK 헤더 대비 */
#endif

#define HOTKEY_ID   1
#define IDT_TRIM    1     /* 활동 후 작업 집합 비우기 일회성 타이머 */
#define TRIM_DELAY_MS 2000
#define WM_TRAYICON (WM_USER + 1)
#define IDM_EXIT      10
#define IDM_LED       11 /* NSS_LED 빌드 전용: 키보드 LED 표시 켜기/끄기 */
#define IDM_AUTOSTART 12 /* 로그인 시 자동 실행 켜기/끄기 */
#define IDM_SHORTCUT  13 /* 시작 메뉴 바로가기 만들기/지우기 */
#define IDI_TRAY    1  /* res/nShiftSpace.rc 의 아이콘 리소스 ID (꺼짐/기본) */
#define IDI_TRAY_ON 2  /* LED 켜짐 아이콘 (NSS_LED 빌드에만 존재) */

static NOTIFYICONDATAW g_nid;
static WNDCLASSW       g_wc;
static UINT            g_taskbar_created;
static WCHAR           g_run_cmd[MAX_PATH + 3];  /* "<exe 경로>" + NUL — Run 값 */

/* 트레이 메뉴 맨 위에 보이는 "이름 v버전" 줄.
 * 버전은 빌드가 -DNSS_VERSION=0.3.2 처럼 따옴표 없이 넘긴다(Makefile · build.bat · CI는 태그 이름).
 * 없으면 "dev". 두 단계 매크로로 L"..." 문자열로 바꾼다. */
#ifndef NSS_VERSION
#define NSS_VERSION dev
#endif
#define NSS_STR2(x)  #x
#define NSS_STR(x)   NSS_STR2(x)
#define NSS_WIDE2(s) L##s
#define NSS_WIDE(s)  NSS_WIDE2(s)
#ifdef NSS_LED
#define APP_TITLE L"nShiftSpace LED v" NSS_WIDE(NSS_STR(NSS_VERSION))
#define LNK_NAME  L"\\nShiftSpace LED.lnk"   /* Chocolatey 패키지의 바로가기 이름과 같다 */
#else
#define APP_TITLE L"nShiftSpace v" NSS_WIDE(NSS_STR(NSS_VERSION))
#define LNK_NAME  L"\\nShiftSpace.lnk"
#endif

#define RUN_KEY L"Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define RUN_VAL L"nShiftSpace"

/* --- 로그인 시 자동 실행 (HKCU Run 값) ---
 * Vista+ 의 RegGetValue / RegSetKeyValue / RegDeleteKeyValue 를 쓰면 키 열기·닫기가
 * 필요 없어 import 3개로 끝난다 (RegOpenKeyEx/RegQueryValueEx/RegSetValueEx/RegDeleteValue/
 * RegCloseKey 5개 대비 코드·import 테이블 절약). */

static BOOL autostart_enabled(void)
{
    return RegGetValueW(HKEY_CURRENT_USER, RUN_KEY, RUN_VAL, RRF_RT_REG_SZ,
                        NULL, NULL, NULL) == ERROR_SUCCESS;
}

static void set_autostart(BOOL on)
{
    if (on) {
        /* 경로에 공백이 있어도 되도록 따옴표로 감싼다: "<exe 경로>" */
        DWORD n = GetModuleFileNameW(NULL, g_run_cmd + 1, MAX_PATH);
        g_run_cmd[0]     = L'"';
        g_run_cmd[n + 1] = L'"';
        g_run_cmd[n + 2] = 0;
        RegSetKeyValueW(HKEY_CURRENT_USER, RUN_KEY, RUN_VAL, REG_SZ,
                        g_run_cmd, (n + 3) * sizeof(WCHAR));
    } else {
        RegDeleteKeyValueW(HKEY_CURRENT_USER, RUN_KEY, RUN_VAL);
    }
}

/* --- 시작 메뉴 바로가기 ---
 * g_lnk 에 시작 메뉴 Programs 폴더의 바로가기 경로를 만들고 그 파일이 있는지 돌려준다.
 * common: FALSE = 사용자(%APPDATA%), TRUE = 공용(%ProgramData%).
 * SHGetFolderPath 대신 환경 변수를 편다 — 셸 폴더 API는 windows.storage 등 큰 DLL을 끌어와
 * 메뉴를 열 때마다 메모리를 늘린다. */
#define PROGRAMS_DIR L"\\Microsoft\\Windows\\Start Menu\\Programs"
static WCHAR g_lnk[MAX_PATH + 32];
static WCHAR g_exe[MAX_PATH];

static BOOL shortcut_exists(BOOL common)
{
    if (!ExpandEnvironmentStringsW(common ? L"%ProgramData%" PROGRAMS_DIR LNK_NAME
                                          : L"%APPDATA%" PROGRAMS_DIR LNK_NAME,
                                   g_lnk, MAX_PATH + 32)) {
        g_lnk[0] = 0;
        return FALSE;
    }
    return GetFileAttributesW(g_lnk) != INVALID_FILE_ATTRIBUTES;
}

/* uuid 라이브러리를 링크하지 않도록 필요한 GUID 셋만 직접 둔다 */
static const GUID k_clsid_shell_link =
    { 0x00021401, 0, 0, { 0xC0, 0, 0, 0, 0, 0, 0, 0x46 } };
static const GUID k_iid_shell_link_w =
    { 0x000214F9, 0, 0, { 0xC0, 0, 0, 0, 0, 0, 0, 0x46 } };
static const GUID k_iid_persist_file =
    { 0x0000010B, 0, 0, { 0xC0, 0, 0, 0, 0, 0, 0, 0x46 } };

typedef HRESULT (WINAPI *co_initialize_t)(LPVOID);
typedef void    (WINAPI *co_uninitialize_t)(void);
typedef HRESULT (WINAPI *co_create_instance_t)(const GUID *, void *, DWORD, const GUID *, void **);

/* g_lnk 경로에 자기 exe 를 가리키는 바로가기를 저장한다. ole32 는 이 함수 안에서만 올라온다. */
static void create_shortcut(void)
{
    HMODULE ole = LoadLibraryW(L"ole32.dll");
    co_initialize_t      co_init;
    co_uninitialize_t    co_uninit;
    co_create_instance_t co_create;
    IShellLinkW  *link;
    IPersistFile *file;

    if (!ole)
        return;
    co_init   = (co_initialize_t)(void *)GetProcAddress(ole, "CoInitialize");
    co_uninit = (co_uninitialize_t)(void *)GetProcAddress(ole, "CoUninitialize");
    co_create = (co_create_instance_t)(void *)GetProcAddress(ole, "CoCreateInstance");
    if (co_init && co_uninit && co_create && SUCCEEDED(co_init(NULL))) {
        if (SUCCEEDED(co_create(&k_clsid_shell_link, NULL, CLSCTX_INPROC_SERVER,
                                &k_iid_shell_link_w, (void **)&link))) {
            GetModuleFileNameW(NULL, g_exe, MAX_PATH);
            link->lpVtbl->SetPath(link, g_exe);
            if (SUCCEEDED(link->lpVtbl->QueryInterface(link, &k_iid_persist_file,
                                                       (void **)&file))) {
                file->lpVtbl->Save(file, g_lnk, TRUE);
                file->lpVtbl->Release(file);
            }
            link->lpVtbl->Release(link);
        }
        co_uninit();
    }
    FreeLibrary(ole);
}

#ifdef NSS_LED
static HICON           g_icon_off, g_icon_on;
static BOOL            g_lit;           /* 현재 트레이 아이콘이 켜짐 상태인가 */
static BOOL            g_led_enabled;   /* LED 기능 자체의 켜짐/꺼짐 (트레이 메뉴) */
static RAWINPUTDEVICE  g_rid;
static RAWINPUT        g_raw;           /* WM_INPUT 수신 버퍼 (키보드 전용 등록이므로 충분) */

/* 상태가 실제로 바뀔 때만 셸에 아이콘 교체를 요청한다 (키 반복 입력 시 호출 억제) */
static void set_led(BOOL on)
{
    if (on == g_lit)
        return;
    g_lit = on;
    g_nid.hIcon = on ? g_icon_on : g_icon_off;
    Shell_NotifyIconW(NIM_MODIFY, &g_nid);
}

/* LED 기능 켜기/끄기 = 키보드 Raw Input 등록/해제.
 * 끄면 WM_INPUT 자체가 오지 않으므로 키 입력당 비용이 기본 빌드와 같은 0이 된다.
 * RIDEV_REMOVE 는 hwndTarget 이 NULL 이어야 한다. */
static void set_led_enabled(HWND hwnd, BOOL enable)
{
    g_led_enabled     = enable;
    g_rid.usUsagePage = 0x01;  /* HID Generic Desktop */
    g_rid.usUsage     = 0x06;  /* Keyboard */
    g_rid.dwFlags     = enable ? RIDEV_INPUTSINK : RIDEV_REMOVE;
    g_rid.hwndTarget  = enable ? hwnd : NULL;
    RegisterRawInputDevices(&g_rid, 1, sizeof(g_rid));
    if (!enable)
        set_led(FALSE);  /* 켜진 채로 끄면 아이콘을 꺼짐으로 되돌린다 */
}
#endif

static void add_tray_icon(HWND hwnd)
{
    g_nid.cbSize           = sizeof(g_nid);
    g_nid.hWnd             = hwnd;
    g_nid.uID              = 1;
    g_nid.uFlags           = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    g_nid.uCallbackMessage = WM_TRAYICON;
#ifdef NSS_LED
    g_lit       = FALSE;   /* 탐색기 재시작으로 다시 추가될 때 꺼짐 상태로 복귀 */
    g_nid.hIcon = g_icon_off;
#else
    g_nid.hIcon            = LoadIconW(GetModuleHandleW(NULL),
                                       MAKEINTRESOURCEW(IDI_TRAY));
#endif
    lstrcpynW(g_nid.szTip, L"nShiftSpace: Shift+Space 한/영 전환", 64);
    Shell_NotifyIconW(NIM_ADD, &g_nid);
}

/* 작업 집합 비우기 — 상주 페이지를 대기 목록으로 내려 Task Manager 수치를 낮춘다.
 * 호출 시점은 WM_TIMER(IDT_TRIM) 한 곳: 어떤 메시지든 받으면 타이머를 (재)설정하고
 * 2초간 조용하면 한 번 비운다. */
static void trim_working_set(void)
{
    /* GetCurrentProcess() 의 의사 핸들 값은 -1 로 고정 — 호출 대신 상수를 써 import 하나를 아낀다 */
    SetProcessWorkingSetSize((HANDLE)(LONG_PTR)-1, (SIZE_T)-1, (SIZE_T)-1);
}

static void send_hangul_key(void)
{
    BYTE scan = (BYTE)MapVirtualKeyW(VK_HANGUL, MAPVK_VK_TO_VSC);
    keybd_event(VK_HANGUL, scan, 0, 0);
    keybd_event(VK_HANGUL, scan, KEYEVENTF_KEYUP, 0);
}

static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    if (msg == WM_TIMER) {
        if (wp == IDT_TRIM) {
            KillTimer(hwnd, IDT_TRIM);
            trim_working_set();
        }
        return 0;
    }
    /* 어떤 메시지든 활동으로 간주: 2초 뒤 한 번 비우도록 일회성 타이머 (재)설정 */
    SetTimer(hwnd, IDT_TRIM, TRIM_DELAY_MS, NULL);

    /* 탐색기(explorer.exe) 재시작 시 트레이 아이콘 복구 */
    if (msg == g_taskbar_created) {
        add_tray_icon(hwnd);
        return 0;
    }

    switch (msg) {
    case WM_HOTKEY:
        send_hangul_key();
        return 0;

#ifdef NSS_LED
    case WM_INPUT: {
        UINT size = sizeof(g_raw);
        if (GetRawInputData((HRAWINPUT)lp, RID_INPUT, &g_raw, &size,
                            sizeof(RAWINPUTHEADER)) != (UINT)-1
            && g_raw.header.dwType == RIM_TYPEKEYBOARD)
            set_led(!(g_raw.data.keyboard.Flags & RI_KEY_BREAK));
        break;  /* WM_INPUT은 DefWindowProc에 넘겨 버퍼를 정리시켜야 한다 */
    }
#endif

    case WM_TRAYICON:
        if (LOWORD(lp) == WM_RBUTTONUP || LOWORD(lp) == WM_LBUTTONUP) {
            POINT pt;
            HMENU menu = CreatePopupMenu();
            /* 맨 위: 프로그램 이름과 버전 (누를 수 없는 머리 줄) */
            AppendMenuW(menu, MF_STRING | MF_GRAYED, 0, APP_TITLE);
            AppendMenuW(menu, MF_SEPARATOR, 0, NULL);
            /* "로그인 시 자동 실행(&S)" — 체크 표시는 Run 값 존재 여부 */
            AppendMenuW(menu, autostart_enabled() ? MF_STRING | MF_CHECKED : MF_STRING,
                        IDM_AUTOSTART, L"로그인 시 자동 실행(&S)");
            /* "시작 메뉴에 바로가기(&M)" — 사용자 또는 공용 시작 메뉴에 있으면 체크.
               공용 쪽에만 있으면(Chocolatey 설치) 우리가 지울 수 없으므로 비활성으로 보여 준다. */
            {
                BOOL common = shortcut_exists(TRUE);
                BOOL user   = shortcut_exists(FALSE);
                AppendMenuW(menu,
                            MF_STRING | ((user || common) ? MF_CHECKED : 0)
                                      | ((common && !user) ? MF_GRAYED : 0),
                            IDM_SHORTCUT, L"시작 메뉴에 바로가기(&M)");
            }
#ifdef NSS_LED
            /* "키보드 LED 표시(&L)" — 체크 표시로 현재 상태를 보여준다 */
            AppendMenuW(menu, g_led_enabled ? MF_STRING | MF_CHECKED : MF_STRING,
                        IDM_LED, L"키보드 LED 표시(&L)");
#endif
            AppendMenuW(menu, MF_SEPARATOR, 0, NULL);
            /* "종료(&X)" */
            AppendMenuW(menu, MF_STRING, IDM_EXIT, L"종료(&X)");
            GetCursorPos(&pt);
            SetForegroundWindow(hwnd);
            TrackPopupMenu(menu, TPM_RIGHTBUTTON | TPM_BOTTOMALIGN,
                           pt.x, pt.y, 0, hwnd, NULL);
            DestroyMenu(menu);
        }
        return 0;

    case WM_COMMAND:
        if (LOWORD(wp) == IDM_EXIT) {
            /* 단축키·Raw Input·창은 프로세스 종료와 함께 OS 가 정리한다. 아이콘만 직접 지운다 */
            Shell_NotifyIconW(NIM_DELETE, &g_nid);
            ExitProcess(0);
        }
        else if (LOWORD(wp) == IDM_AUTOSTART)
            set_autostart(!autostart_enabled());
        else if (LOWORD(wp) == IDM_SHORTCUT) {
            if (shortcut_exists(FALSE))
                DeleteFileW(g_lnk);
            else if (g_lnk[0])
                create_shortcut();
        }
#ifdef NSS_LED
        else if (LOWORD(wp) == IDM_LED)
            set_led_enabled(hwnd, !g_led_enabled);
#endif
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

/* CRT 없는 진입점 (링커 옵션 -e start / -e _start 로 지정) */
void start(void)
{
    HINSTANCE hinst = GetModuleHandleW(NULL);
    HWND      hwnd;
    MSG       msg;

    /* 중복 실행 방지 */
    CreateMutexW(NULL, TRUE, L"nShiftSpace-single-instance");
    if (GetLastError() == ERROR_ALREADY_EXISTS)
        ExitProcess(1);

    /* 이 스레드의 IME/TSF 비활성화 — 창을 만들기 전에 호출해야 한다 */
    ImmDisableIME((DWORD)-1);

    g_taskbar_created = RegisterWindowMessageW(L"TaskbarCreated");

    g_wc.lpfnWndProc   = wnd_proc;
    g_wc.hInstance     = hinst;
    g_wc.lpszClassName = L"nShiftSpace";
    RegisterClassW(&g_wc);

    /* 화면에 표시하지 않는 메시지 수신용 윈도우 */
    hwnd = CreateWindowExW(0, L"nShiftSpace", L"nShiftSpace", 0,
                           0, 0, 0, 0, NULL, NULL, hinst, NULL);

    if (!RegisterHotKey(hwnd, HOTKEY_ID, MOD_SHIFT | MOD_NOREPEAT, VK_SPACE)) {
        /* "Shift+Space 단축키 등록에 실패했습니다." */
        MessageBoxW(NULL,
            L"Shift+Space 단축키 등록에 "
            L"실패했습니다.",
            L"nShiftSpace", MB_ICONERROR);
        ExitProcess(1);
    }

#ifdef NSS_LED
    /* 아이콘은 리소스 공유 핸들이라 해제가 필요 없다 */
    g_icon_off = LoadIconW(hinst, MAKEINTRESOURCEW(IDI_TRAY));
    g_icon_on  = LoadIconW(hinst, MAKEINTRESOURCEW(IDI_TRAY_ON));

    /* 키보드 Raw Input 등록 (RIDEV_INPUTSINK: 포그라운드가 아니어도 수신). 기본 켜짐. */
    set_led_enabled(hwnd, TRUE);
#endif

    add_tray_icon(hwnd);

    /* TranslateMessage 는 WM_CHAR 생성용 — 문자 입력을 받지 않으므로 생략 (import 하나 절약) */
    while (GetMessageW(&msg, NULL, 0, 0))
        DispatchMessageW(&msg);
    ExitProcess(0);
}
