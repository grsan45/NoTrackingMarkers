#ifndef NOTRACKINGMARKERS_NTM_H
#define NOTRACKINGMARKERS_NTM_H

#include <windows.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <strsafe.h>

#include <list>

#include <boost/url.hpp>
#include <boost/json.hpp>

#define ICON_CALLBACK 0xf001
#define MENU_EXIT 1
#define MENU_OPEN_CONFIG 2
#define MENU_RELOAD_CONFIG 3

class Configuration {
public:
    BOOL bAutoStart;
    std::unordered_map<std::string_view, std::list<std::string_view>> hosts_map;
};

static Configuration appConfig;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void HandleClipboardUpdate(HWND hwnd);
void HandleIcon(HWND hwnd, WPARAM wIcon, LPARAM lAction);

void ProcessConfig(const CHAR *pbConfigData, Configuration);
void ProcessURL(boost::urls::url* url);

#endif //NOTRACKINGMARKERS_NTM_H
