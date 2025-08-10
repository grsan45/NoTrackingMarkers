#ifndef UNICODE
#define UNICODE
#define _UNICODE
#endif

#include "ntm.h"
#include "resources.h"
#include <list>

#define ICON_CALLBACK 0xf001

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

    INITCOMMONCONTROLSEX icex = {.dwSize=sizeof(INITCOMMONCONTROLSEX), .dwICC=ICC_STANDARD_CLASSES | ICC_BAR_CLASSES};
    InitCommonControlsEx(&icex);

    const wchar_t* CLASS_NAME = L"NTM";

    WNDCLASS wc = {.lpfnWndProc = WindowProc, .hInstance = hInstance, .lpszClassName = CLASS_NAME};
    RegisterClass(&wc);

    // create invisible window
    HWND hwnd = CreateWindowEx(
            0,
            CLASS_NAME,
            L"No Tracking Markers",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
            nullptr,
            nullptr,
            hInstance,
            nullptr
            );

    if (hwnd == nullptr) return 0;

    // listen to all changes to clipboard
    AddClipboardFormatListener(hwnd);

    GUID gidIcon;
    CoCreateGuid(&gidIcon);

    // All interaction with the app is done with a system tray icon
    NOTIFYICONDATA iconData = {
            .cbSize = sizeof(NOTIFYICONDATA),
            .hWnd = hwnd,
            .uFlags = NIF_MESSAGE | NIF_GUID | NIF_TIP | NIF_SHOWTIP | NIF_ICON,
            .uCallbackMessage = ICON_CALLBACK,
            .szTip = L"No Tracking Markers",
            .guidItem = gidIcon,
    };
    HRESULT hLoadResult = LoadIconMetric(GetModuleHandle(nullptr), MAKEINTRESOURCE(NTM_ICON_SMALL), LIM_SMALL, &iconData.hIcon);
    if (hLoadResult != S_OK) return 0;

    Shell_NotifyIcon(NIM_ADD, &iconData);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            RemoveClipboardFormatListener(hwnd); // not sure if this is actually necessary
            PostQuitMessage(0);
            return 0;
        case WM_CLIPBOARDUPDATE:
            HandleClipboardUpdate(hwnd);
            return 0;
        case ICON_CALLBACK:
            HandleIcon(hwnd, wParam, lParam);
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void CleanupClipboard(HGLOBAL hglb) {
    GlobalUnlock(hglb);
    CloseClipboard();
}

void HandleClipboardUpdate(HWND hwnd) {
    // Stop if we already own the clipboard. This prevents recursive operations.
    if (GetClipboardOwner() == hwnd) return;

    if (!IsClipboardFormatAvailable(CF_TEXT)) return;
    if (!OpenClipboard(hwnd)) return;

    HGLOBAL hClipboardData = GetClipboardData(CF_TEXT);
    if (hClipboardData == nullptr) {
        CleanupClipboard(hClipboardData);
        return;
    }

    // get current content as string
    auto lpClipboardContent = static_cast<LPSTR>(GlobalLock(hClipboardData));
    if (lpClipboardContent == nullptr) {
        CleanupClipboard(hClipboardData);
        return;
    }

    auto result = boost::urls::parse_uri(lpClipboardContent);
    GlobalUnlock(hClipboardData); // no longer needed, go ahead and unlock

    if (result.has_value()) {
        boost::urls::url uri = result.value();
        ProcessURL(&uri);

        // Place the modified url back on the clipboard
        UINT uBufferSize = (uri.buffer().size() + 1) * sizeof(CHAR);

        HGLOBAL hModified = GlobalAlloc(GMEM_MOVEABLE, uBufferSize);
        if (hModified == nullptr) {
            CleanupClipboard(hClipboardData);
            return;
        }

        auto lpModified = static_cast<LPSTR>(GlobalLock(hModified));
        // right now just accessing the raw buffer data, not sure if there is a better way.
        memcpy(lpModified, uri.buffer().data(), uBufferSize);
        GlobalUnlock(hModified);

        EmptyClipboard();
        SetClipboardData(CF_TEXT, hModified);
    }

    CleanupClipboard(hClipboardData);
}

void ProcessURL(boost::urls::url* url) {
    auto params = url->params();

    // TODO: configurable tracking marker key
    if (params.contains("si"))
        params.erase(params.find("si"));
}

void HandleIcon(HWND hwnd, WPARAM wIcon, LPARAM lAction) {
    // only using right click for a context menu, ignore everything else.
    if (lAction != WM_RBUTTONDOWN) return;

    POINT pCursorPos;
    GetCursorPos(&pCursorPos);

    HMENU hActionMenu = CreatePopupMenu();
    AppendMenu(hActionMenu, MF_DISABLED, MENU_OPEN_CONFIG, L"Open configuration file");
    AppendMenu(hActionMenu, MF_DISABLED, MENU_RELOAD_CONFIG, L"Reload configuration");
    AppendMenu(hActionMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenu(hActionMenu, MF_STRING, MENU_EXIT, L"Exit");

    UINT uSelectedItem = TrackPopupMenu(hActionMenu,
                   TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_RETURNCMD,
                   pCursorPos.x, pCursorPos.y,
                   0, hwnd, nullptr
                   );

    // TODO: implement config
    if (uSelectedItem == MENU_EXIT) PostQuitMessage(0);

    DestroyMenu(hActionMenu);
}
