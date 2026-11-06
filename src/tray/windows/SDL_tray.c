/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2026 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#include "SDL_internal.h"

#include "../SDL_tray_utils.h"
#include "../../core/windows/SDL_windows.h"

#include <windowsx.h>
#include <shellapi.h>

#ifndef NOTIFYICON_VERSION_4
#define NOTIFYICON_VERSION_4 4
#endif
#ifndef NIF_SHOWTIP
#define NIF_SHOWTIP 0x00000080
#endif

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_RETRY_TIMER 1

struct SDL_TrayMenu {
    HMENU hMenu;

    int nEntries;
    SDL_TrayEntry **entries;

    SDL_Tray *parent_tray;
    SDL_TrayEntry *parent_entry;
};

struct SDL_TrayEntry {
    SDL_TrayMenu *parent;
    UINT_PTR id;

    char *label_cache;
    bool separator;
    SDL_TrayEntryFlags flags;
    SDL_TrayCallback callback;
    void *userdata;
    SDL_TrayMenu *submenu;
};

struct SDL_Tray {
    NOTIFYICONDATAW nid;
    HWND hwnd;
    HICON icon;
    SDL_TrayMenu *menu;

    void *userdata;
    SDL_TrayClickCallback left_click_callback;
    SDL_TrayClickCallback right_click_callback;
    SDL_TrayClickCallback middle_click_callback;
    bool version4;
};

static UINT_PTR get_next_id(void)
{
    static UINT_PTR next_id = 0;
    return ++next_id;
}

static SDL_TrayEntry *find_entry_in_menu(SDL_TrayMenu *menu, UINT_PTR id)
{
    for (int i = 0; i < menu->nEntries; i++) {
        SDL_TrayEntry *entry = menu->entries[i];

        if (entry->id == id) {
            return entry;
        }

        if (entry->submenu) {
            SDL_TrayEntry *e = find_entry_in_menu(entry->submenu, id);

            if (e) {
                return e;
            }
        }
    }

    return NULL;
}

static SDL_TrayEntry *find_entry_with_id(SDL_Tray *tray, UINT_PTR id)
{
    if (!tray->menu) {
        return NULL;
    }

    return find_entry_in_menu(tray->menu, id);
}

static SDL_Tray *get_tray_for_menu(SDL_TrayMenu *menu)
{
    while (menu) {
        if (menu->parent_tray) {
            return menu->parent_tray;
        }
        menu = menu->parent_entry ? menu->parent_entry->parent : NULL;
    }
    return NULL;
}

static UINT_PTR get_next_menu_item_id(SDL_Tray *tray)
{
    static UINT_PTR next_id = 0;

    if (!tray) {
        return 0;
    }

    // shortcut: allocation can be O(n^2) near 65,534 live entries; use a free-ID bitmap if huge menus become practical.
    for (int i = 0; i < 0xFFFE; ++i) {
        if (++next_id > 0xFFFE) {
            next_id = 1;
        }
        if (!find_entry_with_id(tray, next_id)) {
            return next_id;
        }
    }

    SDL_SetError("No tray menu command IDs are available");
    return 0;
}

static int get_tray_entry_position(SDL_TrayEntry *entry)
{
    SDL_TrayMenu *menu = entry->parent;
    for (int i = 0; i < menu->nEntries; ++i) {
        if (menu->entries[i] == entry) {
            return i;
        }
    }
    return -1;
}

static bool WIN_AddTrayIcon(SDL_Tray *tray)
{
    if (!Shell_NotifyIconW(NIM_ADD, &tray->nid)) {
        tray->version4 = false;
        return false;
    }

    tray->version4 = Shell_NotifyIconW(NIM_SETVERSION, &tray->nid) != FALSE;
    return true;
}

LRESULT CALLBACK TrayWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    SDL_Tray *tray = (SDL_Tray *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
    SDL_TrayEntry *entry = NULL;

    if (!tray) {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    static UINT s_taskbarRestart = 0;
    if (s_taskbarRestart == 0) {
        s_taskbarRestart = RegisterWindowMessage(TEXT("TaskbarCreated"));
    }
    if (s_taskbarRestart && uMsg == s_taskbarRestart) {
        KillTimer(hwnd, ID_TRAY_RETRY_TIMER);
        if (!WIN_AddTrayIcon(tray)) {
            if (!SetTimer(hwnd, ID_TRAY_RETRY_TIMER, 1000, NULL)) {
                SDL_LogWarn(SDL_LOG_CATEGORY_VIDEO, "Couldn't schedule tray icon retry after Explorer restart");
            }
        }
    }

    switch (uMsg) {
        case WM_TRAYICON:
            {
                SDL_Point position;
                bool has_click_position = tray->version4;
                if (tray->version4) {
                    position.x = GET_X_LPARAM(wParam);
                    position.y = GET_Y_LPARAM(wParam);
                } else {
                    POINT cursor;
                    if (GetCursorPos(&cursor)) {
                        position.x = cursor.x;
                        position.y = cursor.y;
                        has_click_position = true;
                    } else {
                        position.x = position.y = -1;
                    }
                }
                const SDL_Point *click_position = has_click_position ? &position : NULL;
                bool show_menu = false;

                switch (LOWORD(lParam)) {
                    case WM_LBUTTONUP:
                    case NIN_SELECT:
                    case NIN_KEYSELECT:
                        if (tray->left_click_callback) {
                            show_menu = SDL_InvokeTrayClick(tray, tray->left_click_callback, tray->userdata, click_position, NULL);
                        } else {
                            show_menu = true;
                        }
                        break;

                    case WM_CONTEXTMENU:
                    case WM_RBUTTONUP:
                        if (tray->right_click_callback) {
                            show_menu = SDL_InvokeTrayClick(tray, tray->right_click_callback, tray->userdata, click_position, NULL);
                        } else {
                            show_menu = true;
                        }
                        break;

                    case WM_MBUTTONUP:
                        if (tray->middle_click_callback) {
                            SDL_InvokeTrayClick(tray, tray->middle_click_callback, tray->userdata, click_position, NULL);
                        }
                        break;
                }

                if (show_menu && IsWindow(hwnd) && (SDL_Tray *)GetWindowLongPtr(hwnd, GWLP_USERDATA) == tray && tray->menu) {
                    POINT cursor;
                    SetForegroundWindow(hwnd);
                    if (!click_position && GetCursorPos(&cursor)) {
                        position.x = cursor.x;
                        position.y = cursor.y;
                    }
                    TrackPopupMenu(tray->menu->hMenu, TPM_BOTTOMALIGN | TPM_RIGHTALIGN, position.x, position.y, 0, hwnd, NULL);
                }
            }
            break;

        case WM_TIMER:
            if (wParam == ID_TRAY_RETRY_TIMER && WIN_AddTrayIcon(tray)) {
                KillTimer(hwnd, ID_TRAY_RETRY_TIMER);
            }
            break;

        case WM_COMMAND:
            entry = find_entry_with_id(tray, LOWORD(wParam));

            if (entry && (entry->flags & SDL_TRAYENTRY_CHECKBOX)) {
                SDL_SetTrayEntryChecked(entry, !SDL_GetTrayEntryChecked(entry));
            }

            if (entry && entry->callback) {
                entry->callback(entry->userdata, entry);
            }
            break;

        case WM_SETTINGCHANGE:
            if (wParam == 0 && lParam != 0 && SDL_wcscmp((wchar_t *)lParam, L"ImmersiveColorSet") == 0) {
                WIN_UpdateDarkModeForHWND(hwnd);
            }
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

static void FreeSDLMenuData(SDL_TrayMenu *menu)
{
    for (int i = 0; i < menu->nEntries; i++) {
        if (menu->entries[i] && menu->entries[i]->submenu) {
            FreeSDLMenuData(menu->entries[i]->submenu);
        }
        SDL_free(menu->entries[i]->label_cache);
        SDL_free(menu->entries[i]);
    }
    SDL_free(menu->entries);
    SDL_free(menu);
}

static wchar_t *escape_label(const char *in)
{
    const char *c;
    char *c2;
    int len = 0;

    for (c = in; *c; c++) {
        len += (*c == '&') ? 2 : 1;
    }

    char *escaped = (char *)SDL_malloc(SDL_strlen(in) + len + 1);
    if (!escaped) {
        SDL_OutOfMemory();
        return NULL;
    }

    for (c = in, c2 = escaped; *c;) {
        if (*c == '&') {
            *c2++ = *c;
        }

        *c2++ = *c++;
    }

    *c2 = '\0';

    wchar_t *out = WIN_UTF8ToStringW(escaped);
    SDL_free(escaped);
    if (!out) {
        SDL_OutOfMemory();
    }

    return out;
}

static HICON load_default_icon(void)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    if (!hInstance) {
        return CopyIcon(LoadIcon(NULL, IDI_APPLICATION));
    }

    const char *hint = SDL_GetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON_SMALL);
    if (hint && *hint) {
        HICON icon = LoadIcon(hInstance, MAKEINTRESOURCE(SDL_atoi(hint)));
        return CopyIcon(icon ? icon : LoadIcon(NULL, IDI_APPLICATION));
    }

    hint = SDL_GetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON);
    if (hint && *hint) {
        HICON icon = LoadIcon(hInstance, MAKEINTRESOURCE(SDL_atoi(hint)));
        return CopyIcon(icon ? icon : LoadIcon(NULL, IDI_APPLICATION));
    }

    return CopyIcon(LoadIcon(NULL, IDI_APPLICATION));
}

void SDL_UpdateTrays(void)
{
}

static void SDL_PropTrayCleanupCb(void *userdata, void *cls)
{
    WNDCLASSEX wcex;

    wcex.hIcon = NULL;
    wcex.hIconSm = NULL;
    HINSTANCE h = GetModuleHandle(NULL);
    if (GetClassInfoEx(h, cls, &wcex)) {
        UnregisterClass(cls, h);
    }
}

static bool SDL_RegisterTrayClass(LPCWSTR className)
{
    SDL_PropertiesID props = SDL_GetGlobalProperties();
    if (!props) {
        return false;
    }
    if (SDL_GetPointerProperty(props, SDL_PROP_TRAY_CLEANUP, NULL) != NULL) {
        return true;
    }

    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.hCursor = NULL;
    wcex.hIcon = NULL;
    wcex.hIconSm = NULL;
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = className;
    wcex.style = 0;
    wcex.hbrBackground = NULL;
    wcex.lpfnWndProc = TrayWindowProc;
    wcex.hInstance = NULL;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;

    if (!RegisterClassEx(&wcex)) {
        return SDL_SetError("Couldn't register tray class");
    }

    SDL_SetPointerPropertyWithCleanup(props, SDL_PROP_TRAY_CLEANUP, (void *)wcex.lpszClassName, SDL_PropTrayCleanupCb, NULL);
    return true;
}

SDL_Tray *SDL_CreateTrayWithProperties(SDL_PropertiesID props)
{
    if (!SDL_IsMainThread()) {
        SDL_SetError("This function should be called on the main thread");
        return NULL;
    }

    SDL_Tray *tray = (SDL_Tray *)SDL_calloc(1, sizeof(*tray));

    if (!tray) {
        return NULL;
    }

    SDL_Surface *icon = (SDL_Surface *)SDL_GetPointerProperty(props, SDL_PROP_TRAY_CREATE_ICON_POINTER, NULL);
    const char *tooltip = SDL_GetStringProperty(props, SDL_PROP_TRAY_CREATE_TOOLTIP_STRING, NULL);

    tray->userdata = SDL_GetPointerProperty(props, SDL_PROP_TRAY_CREATE_USERDATA_POINTER, NULL);
    tray->left_click_callback = (SDL_TrayClickCallback)SDL_GetPointerProperty(props, SDL_PROP_TRAY_CREATE_LEFTCLICK_CALLBACK_POINTER, NULL);
    tray->right_click_callback = (SDL_TrayClickCallback)SDL_GetPointerProperty(props, SDL_PROP_TRAY_CREATE_RIGHTCLICK_CALLBACK_POINTER, NULL);
    tray->middle_click_callback = (SDL_TrayClickCallback)SDL_GetPointerProperty(props, SDL_PROP_TRAY_CREATE_MIDDLECLICK_CALLBACK_POINTER, NULL);

    tray->menu = NULL;
    if (!SDL_RegisterTrayClass(TEXT("SDL_TRAY"))) {
        SDL_SetError("Failed to register SDL_TRAY window class");
        SDL_free(tray);
        return NULL;
    }
    tray->hwnd = CreateWindowEx(0, TEXT("SDL_TRAY"), NULL, WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, NULL, NULL);
    if (!tray->hwnd) {
        SDL_free(tray);
        SDL_SetError("Failed to create tray window");
        return NULL;
    }

    WIN_UpdateDarkModeForHWND(tray->hwnd);

    SDL_zero(tray->nid);
    tray->nid.cbSize = sizeof(NOTIFYICONDATAW);
    tray->nid.hWnd = tray->hwnd;
    tray->nid.uID = (UINT) get_next_id();
    tray->nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP;
    tray->nid.uCallbackMessage = WM_TRAYICON;
    tray->nid.uVersion = NOTIFYICON_VERSION_4;
    if (tooltip) {
        wchar_t *tooltipw = WIN_UTF8ToStringW(tooltip);
        if (!tooltipw) {
            DestroyWindow(tray->hwnd);
            SDL_free(tray);
            SDL_OutOfMemory();
            return NULL;
        }
        SDL_wcslcpy(tray->nid.szTip, tooltipw, sizeof(tray->nid.szTip) / sizeof(*tray->nid.szTip));
        SDL_free(tooltipw);
    }

    if (icon) {
        tray->nid.hIcon = WIN_CreateIconFromSurface(icon);

        if (!tray->nid.hIcon) {
            DestroyWindow(tray->hwnd);
            SDL_free(tray);
            SDL_SetError("Failed to create tray icon");
            return NULL;
        }

        tray->icon = tray->nid.hIcon;
    } else {
        tray->nid.hIcon = load_default_icon();
        tray->icon = tray->nid.hIcon;
    }

    if (!tray->nid.hIcon) {
        DestroyWindow(tray->hwnd);
        SDL_free(tray);
        SDL_SetError("Failed to create tray icon");
        return NULL;
    }

    SetWindowLongPtr(tray->hwnd, GWLP_USERDATA, (LONG_PTR) tray);
    if (!WIN_AddTrayIcon(tray)) {
        SetWindowLongPtr(tray->hwnd, GWLP_USERDATA, 0);
        DestroyIcon(tray->nid.hIcon);
        DestroyWindow(tray->hwnd);
        SDL_free(tray);
        SDL_SetError("Failed to add tray icon");
        return NULL;
    }

    SDL_RegisterTray(tray);

    return tray;
}

SDL_Tray *SDL_CreateTray(SDL_Surface *icon, const char *tooltip)
{
    SDL_Tray *tray;
    SDL_PropertiesID props = SDL_CreateProperties();
    if (!props) {
        return NULL;
    }
    if (icon) {
        SDL_SetPointerProperty(props, SDL_PROP_TRAY_CREATE_ICON_POINTER, icon);
    }
    if (tooltip) {
        SDL_SetStringProperty(props, SDL_PROP_TRAY_CREATE_TOOLTIP_STRING, tooltip);
    }
    tray = SDL_CreateTrayWithProperties(props);
    SDL_DestroyProperties(props);
    return tray;
}

bool SDL_GetTrayRect(SDL_Tray *tray, SDL_Rect *rect)
{
#if defined(_WIN32_WINNT) && _WIN32_WINNT >= 0x0601
    NOTIFYICONIDENTIFIER identifier;
    RECT native_rect;
#endif

    if (!SDL_IsMainThread()) {
        return SDL_SetError("This function should be called on the main thread");
    }
    if (!SDL_ObjectValid(tray, SDL_OBJECT_TYPE_TRAY)) {
        return SDL_InvalidParamError("tray");
    }
    if (!rect) {
        return SDL_InvalidParamError("rect");
    }

#if defined(_WIN32_WINNT) && _WIN32_WINNT >= 0x0601
    SDL_zero(identifier);
    identifier.cbSize = sizeof(identifier);
    identifier.hWnd = tray->hwnd;
    identifier.uID = tray->nid.uID;
    if (FAILED(Shell_NotifyIconGetRect(&identifier, &native_rect))) {
        return false;
    }

    rect->x = native_rect.left;
    rect->y = native_rect.top;
    rect->w = native_rect.right - native_rect.left;
    rect->h = native_rect.bottom - native_rect.top;
    return true;
#else
    return false;
#endif
}

void SDL_SetTrayIcon(SDL_Tray *tray, SDL_Surface *icon)
{
    HICON new_icon;

    if (!SDL_ObjectValid(tray, SDL_OBJECT_TYPE_TRAY)) {
        return;
    }

    new_icon = icon ? WIN_CreateIconFromSurface(icon) : load_default_icon();
    if (!new_icon) {
        SDL_SetError("Couldn't create tray icon");
        return;
    }

    if (tray->icon) {
        DestroyIcon(tray->icon);
    }

    tray->nid.hIcon = new_icon;
    tray->icon = new_icon;

    if (!Shell_NotifyIconW(NIM_MODIFY, &tray->nid)) {
        SDL_SetError("Couldn't update tray icon");
    }
}

void SDL_SetTrayTooltip(SDL_Tray *tray, const char *tooltip)
{
    if (!SDL_ObjectValid(tray, SDL_OBJECT_TYPE_TRAY)) {
        return;
    }

    if (tooltip) {
        wchar_t *tooltipw = WIN_UTF8ToStringW(tooltip);
        if (!tooltipw) {
            SDL_OutOfMemory();
            return;
        }
        SDL_wcslcpy(tray->nid.szTip, tooltipw, sizeof(tray->nid.szTip) / sizeof(*tray->nid.szTip));
        SDL_free(tooltipw);
    } else {
        tray->nid.szTip[0] = '\0';
    }

    if (!Shell_NotifyIconW(NIM_MODIFY, &tray->nid)) {
        SDL_SetError("Couldn't update tray tooltip");
    }
}

SDL_TrayMenu *SDL_CreateTrayMenu(SDL_Tray *tray)
{
    CHECK_PARAM(!SDL_ObjectValid(tray, SDL_OBJECT_TYPE_TRAY)) {
        SDL_InvalidParamError("tray");
        return NULL;
    }
    if (tray->menu) {
        SDL_SetError("Tray menu already exists");
        return NULL;
    }

    SDL_TrayMenu *menu = (SDL_TrayMenu *)SDL_calloc(1, sizeof(*menu));

    if (!menu) {
        return NULL;
    }

    menu->hMenu = CreatePopupMenu();
    if (!menu->hMenu) {
        SDL_SetError("Couldn't create tray menu");
        SDL_free(menu);
        return NULL;
    }
    menu->parent_tray = tray;
    menu->parent_entry = NULL;
    tray->menu = menu;

    return menu;
}

SDL_TrayMenu *SDL_GetTrayMenu(SDL_Tray *tray)
{
    CHECK_PARAM(!SDL_ObjectValid(tray, SDL_OBJECT_TYPE_TRAY)) {
        SDL_InvalidParamError("tray");
        return NULL;
    }

    return tray->menu;
}

SDL_TrayMenu *SDL_CreateTraySubmenu(SDL_TrayEntry *entry)
{
    CHECK_PARAM(!entry) {
        SDL_InvalidParamError("entry");
        return NULL;
    }

    if (!entry->submenu) {
        SDL_SetError("Cannot create submenu for entry not created with SDL_TRAYENTRY_SUBMENU");
        return NULL;
    }

    return entry->submenu;
}

SDL_TrayMenu *SDL_GetTraySubmenu(SDL_TrayEntry *entry)
{
    CHECK_PARAM(!entry) {
        SDL_InvalidParamError("entry");
        return NULL;
    }

    return entry->submenu;
}

const SDL_TrayEntry **SDL_GetTrayEntries(SDL_TrayMenu *menu, int *count)
{
    CHECK_PARAM(!menu) {
        SDL_InvalidParamError("menu");
        return NULL;
    }

    if (count) {
        *count = menu->nEntries;
    }
    return (const SDL_TrayEntry **)menu->entries;
}

void SDL_RemoveTrayEntry(SDL_TrayEntry *entry)
{
    if (!entry) {
        return;
    }

    SDL_TrayMenu *menu = entry->parent;
    int pos = get_tray_entry_position(entry);

    if (pos < 0 || !DeleteMenu(menu->hMenu, (UINT) pos, MF_BYPOSITION)) {
        SDL_SetError("Couldn't destroy tray entry");
        return;
    }

    for (int i = pos; i < menu->nEntries - 1; ++i) {
        menu->entries[i] = menu->entries[i + 1];
    }

    if (entry->submenu) {
        FreeSDLMenuData(entry->submenu);
    }
    SDL_free(entry->label_cache);

    menu->nEntries--;
    menu->entries[menu->nEntries] = NULL;
    SDL_TrayEntry **new_entries = (SDL_TrayEntry **)SDL_realloc(menu->entries, (menu->nEntries + 1) * sizeof(*new_entries));

    /* Not sure why shrinking would fail, but even if it does, we can live with a "too big" array */
    if (new_entries) {
        menu->entries = new_entries;
        menu->entries[menu->nEntries] = NULL;
    }

    SDL_free(entry);
}

SDL_TrayEntry *SDL_InsertTrayEntryAt(SDL_TrayMenu *menu, int pos, const char *label, SDL_TrayEntryFlags flags)
{
    CHECK_PARAM(!menu) {
        SDL_InvalidParamError("menu");
        return NULL;
    }

    CHECK_PARAM(pos < -1 || pos > menu->nEntries) {
        SDL_InvalidParamError("pos");
        return NULL;
    }
    if (!SDL_ValidateTrayEntryFlags(flags, label == NULL)) {
        return NULL;
    }

    int windows_compatible_pos = pos;

    if (pos == -1) {
        pos = menu->nEntries;
    } else if (pos == menu->nEntries) {
        windows_compatible_pos = -1;
    }

    SDL_TrayEntry *entry = (SDL_TrayEntry *)SDL_calloc(1, sizeof(*entry));
    if (!entry) {
        return NULL;
    }

    wchar_t *label_w = NULL;

    if (label && (label_w = escape_label(label)) == NULL) {
        SDL_free(entry);
        return NULL;
    }

    if (label && !(entry->label_cache = SDL_strdup(label))) {
        SDL_OutOfMemory();
        SDL_free(label_w);
        SDL_free(entry);
        return NULL;
    }

    entry->parent = menu;
    entry->separator = (label == NULL);
    entry->flags = flags;
    entry->callback = NULL;
    entry->userdata = NULL;
    entry->submenu = NULL;

    if (label != NULL && flags & SDL_TRAYENTRY_SUBMENU) {
        entry->submenu = (SDL_TrayMenu *)SDL_calloc(1, sizeof(*entry->submenu));
        if (!entry->submenu) {
            SDL_free(entry->label_cache);
            SDL_free(entry);
            SDL_free(label_w);
            return NULL;
        }

        entry->submenu->hMenu = CreatePopupMenu();
        if (!entry->submenu->hMenu) {
            SDL_SetError("Couldn't create tray submenu");
            SDL_free(entry->submenu);
            SDL_free(entry->label_cache);
            SDL_free(entry);
            SDL_free(label_w);
            return NULL;
        }
        entry->submenu->nEntries = 0;
        entry->submenu->entries = NULL;
        entry->submenu->parent_entry = entry;
        entry->submenu->parent_tray = NULL;

        entry->id = (UINT_PTR) entry->submenu->hMenu;
    } else {
        entry->id = label ? get_next_menu_item_id(get_tray_for_menu(menu)) : 0;
        if (label && !entry->id) {
            SDL_free(entry->label_cache);
            SDL_free(entry);
            SDL_free(label_w);
            return NULL;
        }
    }

    SDL_TrayEntry **new_entries = (SDL_TrayEntry **)SDL_realloc(menu->entries, (menu->nEntries + 2) * sizeof(*new_entries));
    if (!new_entries) {
        SDL_free(entry->label_cache);
        SDL_free(entry);
        SDL_free(label_w);
        if (entry->submenu) {
            DestroyMenu(entry->submenu->hMenu);
            SDL_free(entry->submenu);
        }
        return NULL;
    }

    menu->entries = new_entries;
    menu->entries[menu->nEntries] = NULL;

    BOOL inserted;
    if (label == NULL) {
        inserted = InsertMenuW(menu->hMenu, windows_compatible_pos, MF_SEPARATOR | MF_BYPOSITION, entry->id, NULL);
    } else {
        UINT mf = MF_STRING | MF_BYPOSITION;
        if (flags & SDL_TRAYENTRY_SUBMENU) {
            mf = MF_POPUP;
        }

        if (flags & SDL_TRAYENTRY_DISABLED) {
            mf |= MF_DISABLED | MF_GRAYED;
        }

        if (flags & SDL_TRAYENTRY_CHECKED) {
            mf |= MF_CHECKED;
        }

        inserted = InsertMenuW(menu->hMenu, windows_compatible_pos, mf, entry->id, label_w);
    }
    SDL_free(label_w);
    if (!inserted) {
        SDL_SetError("Couldn't insert tray entry");
        if (entry->submenu) {
            DestroyMenu(entry->submenu->hMenu);
            SDL_free(entry->submenu);
        }
        SDL_free(entry->label_cache);
        SDL_free(entry);
        return NULL;
    }

    for (int i = menu->nEntries; i > pos; i--) {
        menu->entries[i] = menu->entries[i - 1];
    }
    menu->entries[pos] = entry;
    menu->nEntries++;
    menu->entries[menu->nEntries] = NULL;

    return entry;
}

void SDL_SetTrayEntryLabel(SDL_TrayEntry *entry, const char *label)
{
    if (!entry || entry->separator != (label == NULL) || !label) {
        return;
    }
    int pos = get_tray_entry_position(entry);
    if (pos < 0) {
        SDL_SetError("Couldn't update tray entry label");
        return;
    }

    char *label_copy = SDL_strdup(label);
    if (!label_copy) {
        SDL_OutOfMemory();
        return;
    }

    wchar_t *label_w = escape_label(label);

    if (!label_w) {
        SDL_free(label_copy);
        return;
    }

    MENUITEMINFOW mii;
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STRING;

    mii.dwTypeData = label_w;
    mii.cch = (UINT) SDL_wcslen(label_w);

    if (!SetMenuItemInfoW(entry->parent->hMenu, (UINT) pos, TRUE, &mii)) {
        SDL_SetError("Couldn't update tray entry label");
    } else {
        SDL_free(entry->label_cache);
        entry->label_cache = label_copy;
        label_copy = NULL;
    }

    SDL_free(label_copy);
    SDL_free(label_w);
}

void SDL_SetTrayEntryShortcut(SDL_TrayEntry *entry, SDL_Scancode scancode, SDL_Keymod mod)
{
}

const char *SDL_GetTrayEntryLabel(SDL_TrayEntry *entry)
{
    CHECK_PARAM(!entry) {
        SDL_InvalidParamError("entry");
        return NULL;
    }

    return entry->separator ? NULL : entry->label_cache;
}

void SDL_SetTrayEntryChecked(SDL_TrayEntry *entry, bool checked)
{
    if (!entry || !(entry->flags & SDL_TRAYENTRY_CHECKBOX)) {
        return;
    }
    int pos = get_tray_entry_position(entry);
    if (pos < 0) {
        SDL_SetError("Couldn't update tray entry state");
        return;
    }

    if (CheckMenuItem(entry->parent->hMenu, (UINT) pos, MF_BYPOSITION | (checked ? MF_CHECKED : MF_UNCHECKED)) == (UINT)-1) {
        SDL_SetError("Couldn't update tray entry state");
    }
}

bool SDL_GetTrayEntryChecked(SDL_TrayEntry *entry)
{
    if (!entry || !(entry->flags & SDL_TRAYENTRY_CHECKBOX)) {
        return false;
    }
    int pos = get_tray_entry_position(entry);
    if (pos < 0) {
        SDL_SetError("Couldn't query tray entry state");
        return false;
    }

    MENUITEMINFOW mii;
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;

    if (!GetMenuItemInfoW(entry->parent->hMenu, (UINT) pos, TRUE, &mii)) {
        SDL_SetError("Couldn't query tray entry state");
        return false;
    }

    return ((mii.fState & MFS_CHECKED) != 0);
}

void SDL_SetTrayEntryEnabled(SDL_TrayEntry *entry, bool enabled)
{
    if (!entry) {
        return;
    }
    int pos = get_tray_entry_position(entry);
    if (pos < 0) {
        SDL_SetError("Couldn't update tray entry state");
        return;
    }

    if (EnableMenuItem(entry->parent->hMenu, (UINT) pos, MF_BYPOSITION | (enabled ? MF_ENABLED : (MF_DISABLED | MF_GRAYED))) == (UINT)-1) {
        SDL_SetError("Couldn't update tray entry state");
    }
}

bool SDL_GetTrayEntryEnabled(SDL_TrayEntry *entry)
{
    if (!entry) {
        return false;
    }
    int pos = get_tray_entry_position(entry);
    if (pos < 0) {
        SDL_SetError("Couldn't query tray entry state");
        return false;
    }

    MENUITEMINFOW mii;
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;

    if (!GetMenuItemInfoW(entry->parent->hMenu, (UINT) pos, TRUE, &mii)) {
        SDL_SetError("Couldn't query tray entry state");
        return false;
    }

    return (mii.fState & (MFS_DISABLED | MFS_GRAYED)) == 0;
}

void SDL_SetTrayEntryCallback(SDL_TrayEntry *entry, SDL_TrayCallback callback, void *userdata)
{
    if (!entry) {
        return;
    }

    entry->callback = callback;
    entry->userdata = userdata;
}

void SDL_ClickTrayEntry(SDL_TrayEntry *entry)
{
	if (!entry || !SDL_GetTrayEntryEnabled(entry)) {
		return;
	}

	if (entry->flags & SDL_TRAYENTRY_CHECKBOX) {
		SDL_SetTrayEntryChecked(entry, !SDL_GetTrayEntryChecked(entry));
	}

	if (entry->callback) {
		entry->callback(entry->userdata, entry);
	}
}

SDL_TrayMenu *SDL_GetTrayEntryParent(SDL_TrayEntry *entry)
{
    CHECK_PARAM(!entry) {
        SDL_InvalidParamError("entry");
        return NULL;
    }

    return entry->parent;
}

SDL_TrayEntry *SDL_GetTrayMenuParentEntry(SDL_TrayMenu *menu)
{
    CHECK_PARAM(!menu) {
        SDL_InvalidParamError("menu");
        return NULL;
    }

    return menu->parent_entry;
}

SDL_Tray *SDL_GetTrayMenuParentTray(SDL_TrayMenu *menu)
{
    CHECK_PARAM(!menu) {
        SDL_InvalidParamError("menu");
        return NULL;
    }

    return menu->parent_tray;
}

void SDL_DestroyTray(SDL_Tray *tray)
{
    if (!SDL_ObjectValid(tray, SDL_OBJECT_TYPE_TRAY)) {
        return;
    }

    SDL_UnregisterTray(tray);

    Shell_NotifyIconW(NIM_DELETE, &tray->nid);

    if (tray->menu) {
        DestroyMenu(tray->menu->hMenu);
        FreeSDLMenuData(tray->menu);
    }

    if (tray->icon) {
        DestroyIcon(tray->icon);
    }

    if (tray->hwnd) {
        SetWindowLongPtr(tray->hwnd, GWLP_USERDATA, 0);
        DestroyWindow(tray->hwnd);
    }

    SDL_free(tray);
}
