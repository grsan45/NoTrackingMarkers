#ifndef NOTRACKINGMARKERS_NTM_H
#define NOTRACKINGMARKERS_NTM_H

#include <windows.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <strsafe.h>

#include <boost/url.hpp>

#define ICON_CALLBACK 0xf001
#define MENU_EXIT 1
#define MENU_OPEN_CONFIG 2
#define MENU_RELOAD_CONFIG 3

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void HandleClipboardUpdate(HWND hwnd);
void HandleIcon(HWND hwnd, WPARAM wIcon, LPARAM lAction);

void ProcessURL(boost::urls::url* url);

#endif //NOTRACKINGMARKERS_NTM_H
