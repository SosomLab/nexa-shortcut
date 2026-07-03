/*
 * nShiftSpace (Nexa ShiftSpace) — Shift+Space 한/영 전환 단일 기능 상주 유틸.
 *
 * 초경량 원칙: CRT를 링크하지 않는다 (-nostdlib, 진입점 start).
 *  - 큰 구조체는 전역(.bss)에 두어 memset 호출 생성을 피한다.
 *  - 라이브러리는 kernel32 / user32 / shell32 만 사용한다.
 */
#include <windows.h>

#ifndef MOD_NOREPEAT
#define MOD_NOREPEAT 0x4000  /* Vista 이전 SDK 헤더 대비 */
#endif

#define HOTKEY_ID   1
#define WM_TRAYICON (WM_USER + 1)
#define IDM_EXIT    10
#define IDI_TRAY    1  /* res/nShiftSpace.rc 의 아이콘 리소스 ID */

static NOTIFYICONDATAW g_nid;
static WNDCLASSW       g_wc;
static UINT            g_taskbar_created;

static void add_tray_icon(HWND hwnd)
{
    g_nid.cbSize           = sizeof(g_nid);
    g_nid.hWnd             = hwnd;
    g_nid.uID              = 1;
    g_nid.uFlags           = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    g_nid.uCallbackMessage = WM_TRAYICON;
    g_nid.hIcon            = LoadIconW(GetModuleHandleW(NULL),
                                       MAKEINTRESOURCEW(IDI_TRAY));
    lstrcpynW(g_nid.szTip, L"nShiftSpace: Shift+Space 한/영 전환", 64);
    Shell_NotifyIconW(NIM_ADD, &g_nid);
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

    case WM_TRAYICON:
        if (LOWORD(lp) == WM_RBUTTONUP || LOWORD(lp) == WM_LBUTTONUP) {
            POINT pt;
            HMENU menu = CreatePopupMenu();
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
        if (LOWORD(wp) == IDM_EXIT)
            DestroyWindow(hwnd);
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

    add_tray_icon(hwnd);

    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    ExitProcess(0);
}
