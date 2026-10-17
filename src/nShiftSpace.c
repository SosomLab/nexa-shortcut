/*
 * nShiftSpace (Nexa ShiftSpace) — Shift+Space 한/영 전환 단일 기능 상주 유틸.
 *
 * 초경량 원칙: CRT를 링크하지 않는다 (-nostdlib, 진입점 start).
 *  - 큰 구조체는 전역(.bss)에 두어 memset 호출 생성을 피한다.
 *  - 초기화 직후와 트레이 메뉴를 닫은 뒤 작업 집합을 비운다(SetProcessWorkingSetSize -1/-1).
 *    user32/shell32 초기화가 건드린 DLL 데이터 페이지(복사-후-쓰기 사설 페이지)가 상주 상태로
 *    남아 작업 관리자 "메모리"(사설 작업 집합)가 1.5MB 안팎으로 보이기 때문. 비우면 대기 목록으로
 *    내려가 수치가 수백 KB로 떨어지며, 다음 단축키 처리 때 필요한 몇 페이지만 소프트 폴트로
 *    돌아온다 (커밋 메모리 자체는 변하지 않는다 — 표시 수치와 상주 페이지만 줄어든다).
 *  - 라이브러리는 kernel32 / user32 / shell32 만 사용한다.
 *    (예외: advapi32 — "로그인 시 자동 실행" 메뉴가 HKCU\...\Run 값을 읽고 쓰는 데만 사용.
 *     모든 프로세스에 이미 로드되는 DLL이라 메모리 추가 부담은 없다.)
 *
 * 트레이 메뉴:
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

#ifndef MOD_NOREPEAT
#define MOD_NOREPEAT 0x4000  /* Vista 이전 SDK 헤더 대비 */
#endif

#define HOTKEY_ID   1
#define WM_TRAYICON (WM_USER + 1)
#define IDM_EXIT      10
#define IDM_LED       11 /* NSS_LED 빌드 전용: 키보드 LED 표시 켜기/끄기 */
#define IDM_AUTOSTART 12 /* 로그인 시 자동 실행 켜기/끄기 */
#define IDI_TRAY    1  /* res/nShiftSpace.rc 의 아이콘 리소스 ID (꺼짐/기본) */
#define IDI_TRAY_ON 2  /* LED 켜짐 아이콘 (NSS_LED 빌드에만 존재) */

static NOTIFYICONDATAW g_nid;
static WNDCLASSW       g_wc;
static UINT            g_taskbar_created;
static WCHAR           g_run_cmd[MAX_PATH + 3];  /* "<exe 경로>" + NUL — Run 값 */

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

/* 작업 집합 비우기 — 상주 페이지를 대기 목록으로 내려 Task Manager 수치를 낮춘다 */
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
            /* "로그인 시 자동 실행(&S)" — 체크 표시는 Run 값 존재 여부 */
            AppendMenuW(menu, autostart_enabled() ? MF_STRING | MF_CHECKED : MF_STRING,
                        IDM_AUTOSTART, L"로그인 시 자동 실행(&S)");
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
            trim_working_set();  /* 메뉴 표시가 끌어들인 페이지 정리 */
        }
        return 0;

    case WM_COMMAND:
        if (LOWORD(wp) == IDM_EXIT)
            DestroyWindow(hwnd);
        else if (LOWORD(wp) == IDM_AUTOSTART)
            set_autostart(!autostart_enabled());
#ifdef NSS_LED
        else if (LOWORD(wp) == IDM_LED)
            set_led_enabled(hwnd, !g_led_enabled);
#endif
        return 0;

    case WM_DESTROY:
        Shell_NotifyIconW(NIM_DELETE, &g_nid);
        UnregisterHotKey(hwnd, HOTKEY_ID);
        PostQuitMessage(0);
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
    trim_working_set();

    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    ExitProcess(0);
}
