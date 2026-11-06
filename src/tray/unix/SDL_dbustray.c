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

/* Special thanks to the kind Hayden Gray (thag_iceman/A1029384756) from the SDL community for his help! */

#include "SDL_internal.h"

#include "../../core/linux/SDL_dbus.h"

#ifdef SDL_USE_LIBDBUS

#include "../../video/SDL_surface_c.h"
#include "../../core/unix/SDL_appid.h"
#include "../SDL_tray_utils.h"
#include "SDL_unixtray.h"
#include <unistd.h>

typedef struct SDL_TrayDriverDBus
{
    SDL_DBusContext *dbus;
} SDL_TrayDriverDBus;

typedef struct SDL_TrayDBus
{
    SDL_DBusContext *dbus;
    DBusConnection *connection;
    char *service_name;

    char *tooltip;
    char *activation_token;
    SDL_Surface *surface;

    SDL_TrayClickCallback l_cb;
    SDL_TrayClickCallback r_cb;
    SDL_TrayClickCallback m_cb;
    void *udata;

    Uint64 watcher_retry_time;
    bool watcher_needs_register;
    bool block;
} SDL_TrayDBus;

typedef struct SDL_TrayMenuDBus
{
    SDL_ListNode *menu;
    const char *menu_path;

    SDL_TrayEntry **array_representation;
} SDL_TrayMenuDBus;

static bool TrayItemIsMenu(SDL_Tray *tray, SDL_TrayDBus *tray_dbus)
{
    SDL_TrayMenuDBus *menu_dbus = tray->menu ? (SDL_TrayMenuDBus *)tray->menu->internal : NULL;
    return menu_dbus && menu_dbus->menu_path && !tray_dbus->l_cb;
}

typedef struct SDL_TrayEntryDBus
{
    SDL_MenuItem *item;
    SDL_TrayMenu *sub_menu;
} SDL_TrayEntryDBus;

#define SNI_INTERFACE         "org.kde.StatusNotifierItem"
#define SNI_WATCHER_SERVICE   "org.kde.StatusNotifierWatcher"
#define SNI_WATCHER_PATH      "/StatusNotifierWatcher"
#define SNI_WATCHER_INTERFACE "org.kde.StatusNotifierWatcher"
#define SNI_OBJECT_PATH       "/StatusNotifierItem"
#define SNI_WATCHER_MATCH     "type='signal',sender='org.freedesktop.DBus',interface='org.freedesktop.DBus',member='NameOwnerChanged',arg0='org.kde.StatusNotifierWatcher'"
static const char *sni_introspect = "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\r\n<node>\r\n  <interface name=\"org.kde.StatusNotifierItem\">\r\n\r\n    <property name=\"Category\" type=\"s\" access=\"read\"/>\r\n    <property name=\"Id\" type=\"s\" access=\"read\"/>\r\n    <property name=\"Title\" type=\"s\" access=\"read\"/>\r\n    <property name=\"Status\" type=\"s\" access=\"read\"/>\r\n    <property name=\"WindowId\" type=\"u\" access=\"read\"/>\r\n\r\n    <!-- An additional path to add to the theme search path to find the icons specified above. -->\r\n    <property name=\"IconThemePath\" type=\"s\" access=\"read\"/>\r\n    <property name=\"Menu\" type=\"o\" access=\"read\"/>\r\n    <property name=\"ItemIsMenu\" type=\"b\" access=\"read\"/>\r\n\r\n\r\n    <!-- main icon -->\r\n    <!-- names are preferred over pixmaps -->\r\n    <property name=\"IconName\" type=\"s\" access=\"read\"/>\r\n\r\n    <!--struct containing width, height and image data-->\r\n    <property name=\"IconPixmap\" type=\"a(iiay)\" access=\"read\">\r\n      <annotation name=\"org.qtproject.QtDBus.QtTypeName\" value=\"KDbusImageVector\"/>\r\n    </property>\r\n\r\n    <property name=\"OverlayIconName\" type=\"s\" access=\"read\"/>\r\n\r\n    <property name=\"OverlayIconPixmap\" type=\"a(iiay)\" access=\"read\">\r\n      <annotation name=\"org.qtproject.QtDBus.QtTypeName\" value=\"KDbusImageVector\"/>\r\n    </property>\r\n\r\n\r\n    <!-- Requesting attention icon -->\r\n    <property name=\"AttentionIconName\" type=\"s\" access=\"read\"/>\r\n\r\n    <!--same definition as image-->\r\n    <property name=\"AttentionIconPixmap\" type=\"a(iiay)\" access=\"read\">\r\n      <annotation name=\"org.qtproject.QtDBus.QtTypeName\" value=\"KDbusImageVector\"/>\r\n    </property>\r\n\r\n    <property name=\"AttentionMovieName\" type=\"s\" access=\"read\"/>\r\n\r\n\r\n\r\n    <!-- tooltip data -->\r\n\r\n    <!--(iiay) is an image-->\r\n    <property name=\"ToolTip\" type=\"(sa(iiay)ss)\" access=\"read\">\r\n      <annotation name=\"org.qtproject.QtDBus.QtTypeName\" value=\"KDbusToolTipStruct\"/>\r\n    </property>\r\n\r\n    <method name=\"ProvideXdgActivationToken\">\r\n        <arg name=\"token\" type=\"s\" direction=\"in\"/>\r\n    </method>\r\n\r\n    <!-- interaction: the systemtray wants the application to do something -->\r\n    <method name=\"ContextMenu\">\r\n        <!-- we\'re passing the coordinates of the icon, so the app knows where to put the popup window -->\r\n        <arg name=\"x\" type=\"i\" direction=\"in\"/>\r\n        <arg name=\"y\" type=\"i\" direction=\"in\"/>\r\n    </method>\r\n\r\n    <method name=\"Activate\">\r\n        <arg name=\"x\" type=\"i\" direction=\"in\"/>\r\n        <arg name=\"y\" type=\"i\" direction=\"in\"/>\r\n    </method>\r\n\r\n    <method name=\"SecondaryActivate\">\r\n        <arg name=\"x\" type=\"i\" direction=\"in\"/>\r\n        <arg name=\"y\" type=\"i\" direction=\"in\"/>\r\n    </method>\r\n\r\n    <method name=\"Scroll\">\r\n      <arg name=\"delta\" type=\"i\" direction=\"in\"/>\r\n      <arg name=\"orientation\" type=\"s\" direction=\"in\"/>\r\n    </method>\r\n\r\n    <!-- Signals: the client wants to change something in the status-->\r\n    <signal name=\"NewTitle\">\r\n    </signal>\r\n\r\n    <signal name=\"NewIcon\">\r\n    </signal>\r\n\r\n    <signal name=\"NewAttentionIcon\">\r\n    </signal>\r\n\r\n    <signal name=\"NewOverlayIcon\">\r\n    </signal>\r\n\r\n    <signal name=\"NewMenu\">\r\n    </signal>\r\n\r\n    <signal name=\"NewToolTip\">\r\n    </signal>\r\n\r\n    <signal name=\"NewStatus\">\r\n      <arg name=\"status\" type=\"s\"/>\r\n    </signal>\r\n\r\n  </interface>\r\n</node>";

static SDL_Surface *ConvertTrayIcon(SDL_Surface *icon)
{
    return icon ? SDL_ConvertSurface(icon, SDL_PIXELFORMAT_ARGB32) : NULL;
}

static void TrayAppendStringProperty(SDL_TrayDriverDBus *driver, DBusMessageIter *dict_iter, const char *key, const char *value)
{
    DBusMessageIter entry_iter, variant_iter;

    driver->dbus->message_iter_open_container(dict_iter, DBUS_TYPE_DICT_ENTRY, NULL, &entry_iter);
    driver->dbus->message_iter_append_basic(&entry_iter, DBUS_TYPE_STRING, &key);
    driver->dbus->message_iter_open_container(&entry_iter, DBUS_TYPE_VARIANT, "s", &variant_iter);
    driver->dbus->message_iter_append_basic(&variant_iter, DBUS_TYPE_STRING, &value);
    driver->dbus->message_iter_close_container(&entry_iter, &variant_iter);
    driver->dbus->message_iter_close_container(dict_iter, &entry_iter);
}

static void TrayAppendEmptyPixmapProperty(SDL_TrayDriverDBus *driver, DBusMessageIter *dict_iter, const char *key)
{
    DBusMessageIter entry_iter, variant_iter, array_iter;

    driver->dbus->message_iter_open_container(dict_iter, DBUS_TYPE_DICT_ENTRY, NULL, &entry_iter);
    driver->dbus->message_iter_append_basic(&entry_iter, DBUS_TYPE_STRING, &key);
    driver->dbus->message_iter_open_container(&entry_iter, DBUS_TYPE_VARIANT, "a(iiay)", &variant_iter);
    driver->dbus->message_iter_open_container(&variant_iter, DBUS_TYPE_ARRAY, "(iiay)", &array_iter);
    driver->dbus->message_iter_close_container(&variant_iter, &array_iter);
    driver->dbus->message_iter_close_container(&entry_iter, &variant_iter);
    driver->dbus->message_iter_close_container(dict_iter, &entry_iter);
}

static void TrayAppendEmptyPixmapVariant(SDL_TrayDriverDBus *driver, DBusMessageIter *iter)
{
    DBusMessageIter variant_iter, array_iter;

    driver->dbus->message_iter_open_container(iter, DBUS_TYPE_VARIANT, "a(iiay)", &variant_iter);
    driver->dbus->message_iter_open_container(&variant_iter, DBUS_TYPE_ARRAY, "(iiay)", &array_iter);
    driver->dbus->message_iter_close_container(&variant_iter, &array_iter);
    driver->dbus->message_iter_close_container(iter, &variant_iter);
}

static void TrayAppendStringVariant(SDL_TrayDriverDBus *driver, DBusMessageIter *iter, const char *value)
{
    DBusMessageIter variant_iter;

    driver->dbus->message_iter_open_container(iter, DBUS_TYPE_VARIANT, "s", &variant_iter);
    driver->dbus->message_iter_append_basic(&variant_iter, DBUS_TYPE_STRING, &value);
    driver->dbus->message_iter_close_container(iter, &variant_iter);
}

static DBusHandlerResult TrayHandleGetAllProps(SDL_Tray *tray, SDL_TrayDBus *tray_dbus, SDL_TrayDriverDBus *driver, DBusMessage *msg)
{
    SDL_TrayMenuDBus *menu_dbus;
    DBusMessageIter iter, dict_iter, entry_iter, variant_iter;
    DBusMessageIter struct_iter, array_iter;
    DBusMessage *reply;
    const char *interface;
    const char *key;
    const char *value;
    const char *empty;
    dbus_uint32_t uint32_val;
    dbus_bool_t bool_value;

    empty = "";
    driver->dbus->message_iter_init(msg, &iter);
    if (driver->dbus->message_iter_get_arg_type(&iter) != DBUS_TYPE_STRING ||
        driver->dbus->message_iter_next(&iter)) {
        reply = driver->dbus->message_new_error(msg, DBUS_ERROR_INVALID_ARGS, "Expected an interface name");
        if (reply) {
            driver->dbus->connection_send(tray_dbus->connection, reply, NULL);
            driver->dbus->message_unref(reply);
        }
        return DBUS_HANDLER_RESULT_HANDLED;
    }
    driver->dbus->message_iter_init(msg, &iter);
    driver->dbus->message_iter_get_basic(&iter, &interface);
    if (SDL_strcmp(interface, SNI_INTERFACE)) {
        DBusMessageIter empty_dict;

        reply = driver->dbus->message_new_method_return(msg);
        if (reply) {
            driver->dbus->message_iter_init_append(reply, &iter);
            driver->dbus->message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "{sv}", &empty_dict);
            driver->dbus->message_iter_close_container(&iter, &empty_dict);
            driver->dbus->connection_send(tray_dbus->connection, reply, NULL);
            driver->dbus->message_unref(reply);
        }
        return DBUS_HANDLER_RESULT_HANDLED;
    }
    reply = driver->dbus->message_new_method_return(msg);
    if (!reply) {
        return DBUS_HANDLER_RESULT_HANDLED;
    }
    driver->dbus->message_iter_init_append(reply, &iter);
    driver->dbus->message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "{sv}", &dict_iter);

    key = "Category";
    value = "ApplicationStatus";
    driver->dbus->message_iter_open_container(&dict_iter, DBUS_TYPE_DICT_ENTRY, NULL, &entry_iter);
    driver->dbus->message_iter_append_basic(&entry_iter, DBUS_TYPE_STRING, &key);
    driver->dbus->message_iter_open_container(&entry_iter, DBUS_TYPE_VARIANT, "s", &variant_iter);
    driver->dbus->message_iter_append_basic(&variant_iter, DBUS_TYPE_STRING, &value);
    driver->dbus->message_iter_close_container(&entry_iter, &variant_iter);
    driver->dbus->message_iter_close_container(&dict_iter, &entry_iter);

    key = "Id";
    value = SDL_GetAppID();
    driver->dbus->message_iter_open_container(&dict_iter, DBUS_TYPE_DICT_ENTRY, NULL, &entry_iter);
    driver->dbus->message_iter_append_basic(&entry_iter, DBUS_TYPE_STRING, &key);
    driver->dbus->message_iter_open_container(&entry_iter, DBUS_TYPE_VARIANT, "s", &variant_iter);
    driver->dbus->message_iter_append_basic(&variant_iter, DBUS_TYPE_STRING, &value);
    driver->dbus->message_iter_close_container(&entry_iter, &variant_iter);
    driver->dbus->message_iter_close_container(&dict_iter, &entry_iter);

    key = "Title";
    driver->dbus->message_iter_open_container(&dict_iter, DBUS_TYPE_DICT_ENTRY, NULL, &entry_iter);
    driver->dbus->message_iter_append_basic(&entry_iter, DBUS_TYPE_STRING, &key);
    driver->dbus->message_iter_open_container(&entry_iter, DBUS_TYPE_VARIANT, "s", &variant_iter);
    driver->dbus->message_iter_append_basic(&variant_iter, DBUS_TYPE_STRING, &empty);
    driver->dbus->message_iter_close_container(&entry_iter, &variant_iter);
    driver->dbus->message_iter_close_container(&dict_iter, &entry_iter);

    key = "Status";
    value = "Active";
    driver->dbus->message_iter_open_container(&dict_iter, DBUS_TYPE_DICT_ENTRY, NULL, &entry_iter);
    driver->dbus->message_iter_append_basic(&entry_iter, DBUS_TYPE_STRING, &key);
    driver->dbus->message_iter_open_container(&entry_iter, DBUS_TYPE_VARIANT, "s", &variant_iter);
    driver->dbus->message_iter_append_basic(&variant_iter, DBUS_TYPE_STRING, &value);
    driver->dbus->message_iter_close_container(&entry_iter, &variant_iter);
    driver->dbus->message_iter_close_container(&dict_iter, &entry_iter);

    key = "IconName";
    driver->dbus->message_iter_open_container(&dict_iter, DBUS_TYPE_DICT_ENTRY, NULL, &entry_iter);
    driver->dbus->message_iter_append_basic(&entry_iter, DBUS_TYPE_STRING, &key);
    driver->dbus->message_iter_open_container(&entry_iter, DBUS_TYPE_VARIANT, "s", &variant_iter);
    driver->dbus->message_iter_append_basic(&variant_iter, DBUS_TYPE_STRING, &empty);
    driver->dbus->message_iter_close_container(&entry_iter, &variant_iter);
    driver->dbus->message_iter_close_container(&dict_iter, &entry_iter);

    TrayAppendStringProperty(driver, &dict_iter, "IconThemePath", empty);
    TrayAppendStringProperty(driver, &dict_iter, "OverlayIconName", empty);
    TrayAppendEmptyPixmapProperty(driver, &dict_iter, "OverlayIconPixmap");
    TrayAppendStringProperty(driver, &dict_iter, "AttentionIconName", empty);
    TrayAppendEmptyPixmapProperty(driver, &dict_iter, "AttentionIconPixmap");
    TrayAppendStringProperty(driver, &dict_iter, "AttentionMovieName", empty);

    key = "WindowId";
    uint32_val = 0;
    driver->dbus->message_iter_open_container(&dict_iter, DBUS_TYPE_DICT_ENTRY, NULL, &entry_iter);
    driver->dbus->message_iter_append_basic(&entry_iter, DBUS_TYPE_STRING, &key);
    driver->dbus->message_iter_open_container(&entry_iter, DBUS_TYPE_VARIANT, "u", &variant_iter);
    driver->dbus->message_iter_append_basic(&variant_iter, DBUS_TYPE_UINT32, &uint32_val);
    driver->dbus->message_iter_close_container(&entry_iter, &variant_iter);
    driver->dbus->message_iter_close_container(&dict_iter, &entry_iter);

    key = "ItemIsMenu";
    menu_dbus = tray->menu ? (SDL_TrayMenuDBus *)tray->menu->internal : NULL;
    bool_value = TrayItemIsMenu(tray, tray_dbus) ? TRUE : FALSE;
    driver->dbus->message_iter_open_container(&dict_iter, DBUS_TYPE_DICT_ENTRY, NULL, &entry_iter);
    driver->dbus->message_iter_append_basic(&entry_iter, DBUS_TYPE_STRING, &key);
    driver->dbus->message_iter_open_container(&entry_iter, DBUS_TYPE_VARIANT, "b", &variant_iter);
    driver->dbus->message_iter_append_basic(&variant_iter, DBUS_TYPE_BOOLEAN, &bool_value);
    driver->dbus->message_iter_close_container(&entry_iter, &variant_iter);
    driver->dbus->message_iter_close_container(&dict_iter, &entry_iter);

    if (menu_dbus && menu_dbus->menu_path) {
        key = "Menu";
        value = menu_dbus->menu_path;
        driver->dbus->message_iter_open_container(&dict_iter, DBUS_TYPE_DICT_ENTRY, NULL, &entry_iter);
        driver->dbus->message_iter_append_basic(&entry_iter, DBUS_TYPE_STRING, &key);
        driver->dbus->message_iter_open_container(&entry_iter, DBUS_TYPE_VARIANT, "o", &variant_iter);
        driver->dbus->message_iter_append_basic(&variant_iter, DBUS_TYPE_OBJECT_PATH, &value);
        driver->dbus->message_iter_close_container(&entry_iter, &variant_iter);
        driver->dbus->message_iter_close_container(&dict_iter, &entry_iter);
    } else {
        key = "Menu";
        value = "/NO_DBUSMENU";
        driver->dbus->message_iter_open_container(&dict_iter, DBUS_TYPE_DICT_ENTRY, NULL, &entry_iter);
        driver->dbus->message_iter_append_basic(&entry_iter, DBUS_TYPE_STRING, &key);
        driver->dbus->message_iter_open_container(&entry_iter, DBUS_TYPE_VARIANT, "o", &variant_iter);
        driver->dbus->message_iter_append_basic(&variant_iter, DBUS_TYPE_OBJECT_PATH, &value);
        driver->dbus->message_iter_close_container(&entry_iter, &variant_iter);
        driver->dbus->message_iter_close_container(&dict_iter, &entry_iter);
    }

    {
        DBusMessageIter pixmap_array_iter, pixmap_struct_iter, pixmap_byte_array_iter;

        key = "IconPixmap";
        driver->dbus->message_iter_open_container(&dict_iter, DBUS_TYPE_DICT_ENTRY, NULL, &entry_iter);
        driver->dbus->message_iter_append_basic(&entry_iter, DBUS_TYPE_STRING, &key);
        driver->dbus->message_iter_open_container(&entry_iter, DBUS_TYPE_VARIANT, "a(iiay)", &variant_iter);
        driver->dbus->message_iter_open_container(&variant_iter, DBUS_TYPE_ARRAY, "(iiay)", &pixmap_array_iter);
        if (tray_dbus->surface) {
            driver->dbus->message_iter_open_container(&pixmap_array_iter, DBUS_TYPE_STRUCT, NULL, &pixmap_struct_iter);
            driver->dbus->message_iter_append_basic(&pixmap_struct_iter, DBUS_TYPE_INT32, &tray_dbus->surface->w);
            driver->dbus->message_iter_append_basic(&pixmap_struct_iter, DBUS_TYPE_INT32, &tray_dbus->surface->h);
            driver->dbus->message_iter_open_container(&pixmap_struct_iter, DBUS_TYPE_ARRAY, "y", &pixmap_byte_array_iter);
            driver->dbus->message_iter_append_fixed_array(&pixmap_byte_array_iter, DBUS_TYPE_BYTE, &tray_dbus->surface->pixels, tray_dbus->surface->pitch * tray_dbus->surface->h);
            driver->dbus->message_iter_close_container(&pixmap_struct_iter, &pixmap_byte_array_iter);
            driver->dbus->message_iter_close_container(&pixmap_array_iter, &pixmap_struct_iter);
        }
        driver->dbus->message_iter_close_container(&variant_iter, &pixmap_array_iter);
        driver->dbus->message_iter_close_container(&entry_iter, &variant_iter);
        driver->dbus->message_iter_close_container(&dict_iter, &entry_iter);
    }

    {
        value = tray_dbus->tooltip ? tray_dbus->tooltip : empty;
        key = "ToolTip";
        driver->dbus->message_iter_open_container(&dict_iter, DBUS_TYPE_DICT_ENTRY, NULL, &entry_iter);
        driver->dbus->message_iter_append_basic(&entry_iter, DBUS_TYPE_STRING, &key);
        driver->dbus->message_iter_open_container(&entry_iter, DBUS_TYPE_VARIANT, "(sa(iiay)ss)", &variant_iter);
        driver->dbus->message_iter_open_container(&variant_iter, DBUS_TYPE_STRUCT, NULL, &struct_iter);
        driver->dbus->message_iter_append_basic(&struct_iter, DBUS_TYPE_STRING, &empty);
        driver->dbus->message_iter_open_container(&struct_iter, DBUS_TYPE_ARRAY, "(iiay)", &array_iter);
        driver->dbus->message_iter_close_container(&struct_iter, &array_iter);
        driver->dbus->message_iter_append_basic(&struct_iter, DBUS_TYPE_STRING, &value);
        driver->dbus->message_iter_append_basic(&struct_iter, DBUS_TYPE_STRING, &empty);
        driver->dbus->message_iter_close_container(&variant_iter, &struct_iter);
        driver->dbus->message_iter_close_container(&entry_iter, &variant_iter);
        driver->dbus->message_iter_close_container(&dict_iter, &entry_iter);
    }

    driver->dbus->message_iter_close_container(&iter, &dict_iter);
    driver->dbus->connection_send(tray_dbus->connection, reply, NULL);
    driver->dbus->message_unref(reply);
    return DBUS_HANDLER_RESULT_HANDLED;
}

static DBusHandlerResult TrayHandleGetProp(SDL_Tray *tray, SDL_TrayDBus *tray_dbus, SDL_TrayDriverDBus *driver, DBusMessage *msg)
{
    SDL_TrayMenuDBus *menu_dbus;
    DBusMessageIter iter, variant_iter;
    DBusMessage *reply;
    const char *interface, *property;
    const char *value;
    const char *empty;
    dbus_bool_t bool_value;

    menu_dbus = tray->menu ? (SDL_TrayMenuDBus *)tray->menu->internal : NULL;

    empty = "";
    driver->dbus->message_iter_init(msg, &iter);
    if (driver->dbus->message_iter_get_arg_type(&iter) != DBUS_TYPE_STRING ||
        !driver->dbus->message_iter_next(&iter) || driver->dbus->message_iter_get_arg_type(&iter) != DBUS_TYPE_STRING ||
        driver->dbus->message_iter_next(&iter)) {
        reply = driver->dbus->message_new_error(msg, DBUS_ERROR_INVALID_ARGS, "Expected interface and property names");
        if (reply) {
            driver->dbus->connection_send(tray_dbus->connection, reply, NULL);
            driver->dbus->message_unref(reply);
        }
        return DBUS_HANDLER_RESULT_HANDLED;
    }
    driver->dbus->message_iter_init(msg, &iter);
    driver->dbus->message_iter_get_basic(&iter, &interface);
    driver->dbus->message_iter_next(&iter);
    driver->dbus->message_iter_get_basic(&iter, &property);

    if (SDL_strcmp(interface, SNI_INTERFACE)) {
        reply = driver->dbus->message_new_error(msg, DBUS_ERROR_UNKNOWN_INTERFACE, "Unknown interface");
        if (reply) {
            driver->dbus->connection_send(tray_dbus->connection, reply, NULL);
            driver->dbus->message_unref(reply);
        }
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    reply = driver->dbus->message_new_method_return(msg);
    if (!reply) {
        return DBUS_HANDLER_RESULT_HANDLED;
    }
    driver->dbus->message_iter_init_append(reply, &iter);

    if (!SDL_strcmp(property, "Category")) {
        value = "ApplicationStatus";
        driver->dbus->message_iter_open_container(&iter, DBUS_TYPE_VARIANT, "s", &variant_iter);
        driver->dbus->message_iter_append_basic(&variant_iter, DBUS_TYPE_STRING, &value);
        driver->dbus->message_iter_close_container(&iter, &variant_iter);
    } else if (!SDL_strcmp(property, "Id")) {
		value = SDL_GetAppID();
        driver->dbus->message_iter_open_container(&iter, DBUS_TYPE_VARIANT, "s", &variant_iter);
        driver->dbus->message_iter_append_basic(&variant_iter, DBUS_TYPE_STRING, &value);
        driver->dbus->message_iter_close_container(&iter, &variant_iter);
    } else if (!SDL_strcmp(property, "Title")) {
        driver->dbus->message_iter_open_container(&iter, DBUS_TYPE_VARIANT, "s", &variant_iter);
        driver->dbus->message_iter_append_basic(&variant_iter, DBUS_TYPE_STRING, &empty);
        driver->dbus->message_iter_close_container(&iter, &variant_iter);
    } else if (!SDL_strcmp(property, "Status")) {
        value = "Active";
        driver->dbus->message_iter_open_container(&iter, DBUS_TYPE_VARIANT, "s", &variant_iter);
        driver->dbus->message_iter_append_basic(&variant_iter, DBUS_TYPE_STRING, &value);
        driver->dbus->message_iter_close_container(&iter, &variant_iter);
    } else if (!SDL_strcmp(property, "IconName")) {
        driver->dbus->message_iter_open_container(&iter, DBUS_TYPE_VARIANT, "s", &variant_iter);
        driver->dbus->message_iter_append_basic(&variant_iter, DBUS_TYPE_STRING, &empty);
        driver->dbus->message_iter_close_container(&iter, &variant_iter);
    } else if (!SDL_strcmp(property, "IconThemePath") ||
               !SDL_strcmp(property, "OverlayIconName") ||
               !SDL_strcmp(property, "AttentionIconName") ||
               !SDL_strcmp(property, "AttentionMovieName")) {
        TrayAppendStringVariant(driver, &iter, empty);
    } else if (!SDL_strcmp(property, "OverlayIconPixmap") ||
               !SDL_strcmp(property, "AttentionIconPixmap")) {
        TrayAppendEmptyPixmapVariant(driver, &iter);
    } else if (!SDL_strcmp(property, "ItemIsMenu")) {
        bool_value = TrayItemIsMenu(tray, tray_dbus) ? TRUE : FALSE;
        driver->dbus->message_iter_open_container(&iter, DBUS_TYPE_VARIANT, "b", &variant_iter);
        driver->dbus->message_iter_append_basic(&variant_iter, DBUS_TYPE_BOOLEAN, &bool_value);
        driver->dbus->message_iter_close_container(&iter, &variant_iter);
    } else if (!SDL_strcmp(property, "Menu")) {
        if (menu_dbus && menu_dbus->menu_path) {
            value = menu_dbus->menu_path;
            driver->dbus->message_iter_open_container(&iter, DBUS_TYPE_VARIANT, "o", &variant_iter);
            driver->dbus->message_iter_append_basic(&variant_iter, DBUS_TYPE_OBJECT_PATH, &value);
            driver->dbus->message_iter_close_container(&iter, &variant_iter);
        } else {
            value = "/NO_DBUSMENU";
            driver->dbus->message_iter_open_container(&iter, DBUS_TYPE_VARIANT, "o", &variant_iter);
            driver->dbus->message_iter_append_basic(&variant_iter, DBUS_TYPE_OBJECT_PATH, &value);
            driver->dbus->message_iter_close_container(&iter, &variant_iter);
        }
    } else if (!SDL_strcmp(property, "IconPixmap")) {
        DBusMessageIter array_iter, struct_iter;
        DBusMessageIter byte_array_iter;

        driver->dbus->message_iter_open_container(&iter, DBUS_TYPE_VARIANT, "a(iiay)", &variant_iter);
        driver->dbus->message_iter_open_container(&variant_iter, DBUS_TYPE_ARRAY, "(iiay)", &array_iter);
        if (tray_dbus->surface) {
            driver->dbus->message_iter_open_container(&array_iter, DBUS_TYPE_STRUCT, NULL, &struct_iter);
            driver->dbus->message_iter_append_basic(&struct_iter, DBUS_TYPE_INT32, &tray_dbus->surface->w);
            driver->dbus->message_iter_append_basic(&struct_iter, DBUS_TYPE_INT32, &tray_dbus->surface->h);
            driver->dbus->message_iter_open_container(&struct_iter, DBUS_TYPE_ARRAY, "y", &byte_array_iter);
            driver->dbus->message_iter_append_fixed_array(&byte_array_iter, DBUS_TYPE_BYTE, &tray_dbus->surface->pixels, tray_dbus->surface->pitch * tray_dbus->surface->h);
            driver->dbus->message_iter_close_container(&struct_iter, &byte_array_iter);
            driver->dbus->message_iter_close_container(&array_iter, &struct_iter);
        }
        driver->dbus->message_iter_close_container(&variant_iter, &array_iter);
        driver->dbus->message_iter_close_container(&iter, &variant_iter);
    } else if (!SDL_strcmp(property, "ToolTip")) {
        DBusMessageIter struct_iter, array_iter;
        value = tray_dbus->tooltip ? tray_dbus->tooltip : empty;

        driver->dbus->message_iter_open_container(&iter, DBUS_TYPE_VARIANT, "(sa(iiay)ss)", &variant_iter);
        driver->dbus->message_iter_open_container(&variant_iter, DBUS_TYPE_STRUCT, NULL, &struct_iter);
        driver->dbus->message_iter_append_basic(&struct_iter, DBUS_TYPE_STRING, &empty);
        driver->dbus->message_iter_open_container(&struct_iter, DBUS_TYPE_ARRAY, "(iiay)", &array_iter);
        driver->dbus->message_iter_close_container(&struct_iter, &array_iter);
        driver->dbus->message_iter_append_basic(&struct_iter, DBUS_TYPE_STRING, &value);
        driver->dbus->message_iter_append_basic(&struct_iter, DBUS_TYPE_STRING, &empty);
        driver->dbus->message_iter_close_container(&variant_iter, &struct_iter);
        driver->dbus->message_iter_close_container(&iter, &variant_iter);
    } else if (!SDL_strcmp(property, "WindowId")) {
        dbus_uint32_t uint32_val;

        uint32_val = 0;
        driver->dbus->message_iter_open_container(&iter, DBUS_TYPE_VARIANT, "u", &variant_iter);
        driver->dbus->message_iter_append_basic(&variant_iter, DBUS_TYPE_UINT32, &uint32_val);
        driver->dbus->message_iter_close_container(&iter, &variant_iter);
    } else {
        driver->dbus->message_unref(reply);
        reply = driver->dbus->message_new_error(msg, DBUS_ERROR_UNKNOWN_PROPERTY, "Unknown property");
    }

    if (reply) {
        driver->dbus->connection_send(tray_dbus->connection, reply, NULL);
        driver->dbus->message_unref(reply);
    }
    return DBUS_HANDLER_RESULT_HANDLED;
}

static DBusHandlerResult TrayMessageHandler(DBusConnection *connection, DBusMessage *msg, void *user_data)
{
    SDL_Tray *tray;
    SDL_TrayDBus *tray_dbus;
    SDL_TrayDriverDBus *driver;
    DBusMessage *reply;

    tray = user_data;
    tray_dbus = (SDL_TrayDBus *)tray->internal;
    driver = (SDL_TrayDriverDBus *)tray->driver->internal;

    if (driver->dbus->message_is_method_call(msg, "org.freedesktop.DBus.Properties", "Get")) {
        return TrayHandleGetProp(tray, tray_dbus, driver, msg);
    } else if (driver->dbus->message_is_method_call(msg, "org.freedesktop.DBus.Properties", "GetAll")) {
        return TrayHandleGetAllProps(tray, tray_dbus, driver, msg);
    } else if (driver->dbus->message_is_method_call(msg, "org.freedesktop.DBus.Introspectable", "Introspect")) {
        reply = driver->dbus->message_new_method_return(msg);
        if (!reply) {
            return DBUS_HANDLER_RESULT_HANDLED;
        }
        driver->dbus->message_append_args(reply, DBUS_TYPE_STRING, &sni_introspect, DBUS_TYPE_INVALID);
        driver->dbus->connection_send(tray_dbus->connection, reply, NULL);
        driver->dbus->message_unref(reply);
        return DBUS_HANDLER_RESULT_HANDLED;
    } else if (driver->dbus->message_is_method_call(msg, SNI_INTERFACE, "ProvideXdgActivationToken")) {
        const char *token;
        if (!driver->dbus->message_get_args(msg, NULL, DBUS_TYPE_STRING, &token, DBUS_TYPE_INVALID)) {
            reply = driver->dbus->message_new_error(msg, DBUS_ERROR_INVALID_ARGS, "Expected an activation token string");
        } else {
            char *copy = SDL_strdup(token);
            if (!copy) {
                reply = driver->dbus->message_new_error(msg, DBUS_ERROR_NO_MEMORY, "Unable to copy activation token");
            } else {
                SDL_free(tray_dbus->activation_token);
                tray_dbus->activation_token = copy;
                reply = driver->dbus->message_new_method_return(msg);
            }
        }
        if (reply) {
            driver->dbus->connection_send(connection, reply, NULL);
            driver->dbus->message_unref(reply);
        }
        return DBUS_HANDLER_RESULT_HANDLED;
    } else if (driver->dbus->message_is_method_call(msg, SNI_INTERFACE, "ContextMenu") ||
               driver->dbus->message_is_method_call(msg, SNI_INTERFACE, "Activate") ||
               driver->dbus->message_is_method_call(msg, SNI_INTERFACE, "SecondaryActivate")) {
        dbus_int32_t x, y;
        if (!driver->dbus->message_get_args(msg, NULL, DBUS_TYPE_INT32, &x, DBUS_TYPE_INT32, &y, DBUS_TYPE_INVALID)) {
            reply = driver->dbus->message_new_error(msg, DBUS_ERROR_INVALID_ARGS, "Expected screen coordinates");
            if (reply) {
                driver->dbus->connection_send(connection, reply, NULL);
                driver->dbus->message_unref(reply);
            }
            return DBUS_HANDLER_RESULT_HANDLED;
        }
        SDL_TrayClickCallback callback = tray_dbus->l_cb;
        if (driver->dbus->message_is_method_call(msg, SNI_INTERFACE, "ContextMenu")) {
            callback = tray_dbus->r_cb;
        } else if (driver->dbus->message_is_method_call(msg, SNI_INTERFACE, "SecondaryActivate")) {
            callback = tray_dbus->m_cb;
        }
        void *userdata = tray_dbus->udata;
        char *token = tray_dbus->activation_token;
        tray_dbus->activation_token = NULL;
        SDL_Point position = { x, y };
        // Reply before user code, which may destroy the tray and its connection.
        reply = driver->dbus->message_new_method_return(msg);
        if (reply) {
            driver->dbus->connection_send(connection, reply, NULL);
            driver->dbus->message_unref(reply);
        }
        SDL_InvokeTrayClick(tray, callback, userdata, &position, token);
        SDL_free(token);
        return DBUS_HANDLER_RESULT_HANDLED;
    } else if (driver->dbus->message_is_method_call(msg, SNI_INTERFACE, "Scroll")) {
        DBusError err;
        char *orientation;
        Sint32 delta;

        driver->dbus->error_init(&err);
        driver->dbus->message_get_args(msg, &err, DBUS_TYPE_INT32, &delta, DBUS_TYPE_STRING, &orientation, DBUS_TYPE_INVALID);
        if (driver->dbus->error_is_set(&err)) {
            driver->dbus->error_free(&err);
            reply = driver->dbus->message_new_error(msg, DBUS_ERROR_INVALID_ARGS, "Expected scroll delta and orientation");
            if (reply) {
                driver->dbus->connection_send(tray_dbus->connection, reply, NULL);
                driver->dbus->message_unref(reply);
            }
            return DBUS_HANDLER_RESULT_HANDLED;
        } else {
            driver->dbus->error_free(&err);
        }

        if (SDL_strcmp(orientation, "horizontal") && SDL_strcmp(orientation, "vertical")) {
            reply = driver->dbus->message_new_error(msg, DBUS_ERROR_INVALID_ARGS, "Expected horizontal or vertical orientation");
            if (reply) {
                driver->dbus->connection_send(tray_dbus->connection, reply, NULL);
                driver->dbus->message_unref(reply);
            }
            return DBUS_HANDLER_RESULT_HANDLED;
        }

        reply = driver->dbus->message_new_method_return(msg);
        if (reply) {
            driver->dbus->connection_send(tray_dbus->connection, reply, NULL);
            driver->dbus->message_unref(reply);
        }
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

static DBusHandlerResult TrayWatcherOwnerChanged(DBusConnection *connection, DBusMessage *msg, void *userdata)
{
    SDL_TrayDBus *tray_dbus = userdata;
    const char *name, *old_owner, *new_owner;
    (void)connection;

    if (!tray_dbus->dbus->message_is_signal(msg, "org.freedesktop.DBus", "NameOwnerChanged") ||
        !tray_dbus->dbus->message_get_args(msg, NULL,
                                           DBUS_TYPE_STRING, &name,
                                           DBUS_TYPE_STRING, &old_owner,
                                           DBUS_TYPE_STRING, &new_owner,
                                           DBUS_TYPE_INVALID) ||
        SDL_strcmp(name, SNI_WATCHER_SERVICE)) {
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    tray_dbus->watcher_needs_register = (*new_owner != '\0');
    tray_dbus->watcher_retry_time = 0;
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

SDL_Tray *CreateTray(SDL_TrayDriver *driver, SDL_PropertiesID props)
{
    SDL_TrayDriverDBus *dbus_driver;
    SDL_TrayDBus *tray_dbus;
    SDL_Tray *tray;
    SDL_Surface *icon;
    const char *tooltip;
    const char *object_path;
    char *register_name;
    DBusObjectPathVTable vtable;
    DBusError err;
    int status;
    dbus_bool_t bool_status;
#define CLEANUP()                           \
    SDL_free(tray_dbus->tooltip);           \
    SDL_free(tray_dbus->service_name);      \
    SDL_DestroySurface(tray_dbus->surface); \
    SDL_free(tray);                         \
    SDL_free(tray_dbus)
#define CLEANUP2()                                                     \
    if (tray_dbus->connection) {                                       \
        dbus_driver->dbus->connection_close(tray_dbus->connection);    \
        dbus_driver->dbus->connection_unref(tray_dbus->connection);    \
    }                                                                  \
    CLEANUP()

    /* Get properties */
    tooltip = SDL_GetStringProperty(props, SDL_PROP_TRAY_CREATE_TOOLTIP_STRING, NULL);
    icon = (SDL_Surface *)SDL_GetPointerProperty(props, SDL_PROP_TRAY_CREATE_ICON_POINTER, NULL);

    /* Allocate the tray structure */
    tray_dbus = SDL_calloc(1, sizeof(SDL_TrayDBus));
    if (!tray_dbus) {
        return NULL;
    }
    tray = SDL_malloc(sizeof(SDL_Tray));
    if (!tray) {
        CLEANUP();
        return NULL;
    }

    /* Populate */
    tray->menu = NULL;
    tray->driver = driver;
    tray->internal = tray_dbus;
    if (tooltip) {
        tray_dbus->tooltip = SDL_strdup(tooltip);
        if (!tray_dbus->tooltip) {
            CLEANUP();
            return NULL;
        }
    } else {
        tray_dbus->tooltip = NULL;
    }
    tray_dbus->surface = ConvertTrayIcon(icon);
    if (icon && !tray_dbus->surface) {
        CLEANUP();
        return NULL;
    }
    tray_dbus->block = false;

    /* Connect */
    dbus_driver = (SDL_TrayDriverDBus *)driver->internal;
    dbus_driver->dbus->error_init(&err);
    tray_dbus->connection = dbus_driver->dbus->bus_get_private(DBUS_BUS_SESSION, &err);
    if (dbus_driver->dbus->error_is_set(&err)) {
        SDL_SetError("Unable to create tray: %s", err.message);
        dbus_driver->dbus->error_free(&err);
        CLEANUP2();
        return NULL;
    }
    if (!tray_dbus->connection) {
        SDL_SetError("Unable to create tray: unable to get connection!");
        CLEANUP();
        return NULL;
    }

    /* Request name */
    driver->count++;
    if (SDL_GetSandbox() == SDL_SANDBOX_FLATPAK) {
        SDL_asprintf(&tray_dbus->service_name, "%s.tray%d", SDL_GetAppID(), driver->count);
    } else {
        SDL_asprintf(&tray_dbus->service_name, "org.kde.StatusNotifierItem-%d-%d", getpid(), driver->count);
    }
    if (!tray_dbus->service_name) {
        CLEANUP2();
        return NULL;
    }
    status = dbus_driver->dbus->bus_request_name(tray_dbus->connection, tray_dbus->service_name, DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
    if (dbus_driver->dbus->error_is_set(&err)) {
        SDL_SetError("Unable to create tray: %s", err.message);
        dbus_driver->dbus->error_free(&err);
        CLEANUP2();
        return NULL;
    }
    if (status != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        SDL_SetError("Unable to create tray: unable to request a unique name!");
        CLEANUP2();
        return NULL;
    }

    tray_dbus->dbus = dbus_driver->dbus;
    dbus_driver->dbus->error_init(&err);
    dbus_driver->dbus->bus_add_match(tray_dbus->connection, SNI_WATCHER_MATCH, &err);
    if (dbus_driver->dbus->error_is_set(&err)) {
        SDL_SetError("Unable to monitor tray host: %s", err.message);
        dbus_driver->dbus->error_free(&err);
        CLEANUP2();
        return NULL;
    }
    if (!dbus_driver->dbus->connection_add_filter(tray_dbus->connection, TrayWatcherOwnerChanged, tray_dbus, NULL)) {
        SDL_SetError("Unable to monitor tray host changes");
        dbus_driver->dbus->error_init(&err);
        dbus_driver->dbus->bus_remove_match(tray_dbus->connection, SNI_WATCHER_MATCH, &err);
        if (dbus_driver->dbus->error_is_set(&err)) {
            dbus_driver->dbus->error_free(&err);
        }
        CLEANUP2();
        return NULL;
    }

    /* Create object */
    object_path = SNI_OBJECT_PATH;
    SDL_zero(vtable);
    vtable.message_function = TrayMessageHandler;
    bool_status = dbus_driver->dbus->connection_try_register_object_path(tray_dbus->connection, object_path, &vtable, tray, &err);
    if (dbus_driver->dbus->error_is_set(&err)) {
        SDL_SetError("Unable to create tray: %s", err.message);
        dbus_driver->dbus->error_free(&err);
        CLEANUP2();
        return NULL;
    }
    if (!bool_status) {
        SDL_SetError("Unable to create tray: unable to register object path!");
        CLEANUP2();
        return NULL;
    }

    /* Register */
    register_name = tray_dbus->service_name;
    if (!SDL_DBus_CallVoidMethodOnConnection(tray_dbus->connection, SNI_WATCHER_SERVICE, SNI_WATCHER_PATH, SNI_WATCHER_INTERFACE, "RegisterStatusNotifierItem", DBUS_TYPE_STRING, &register_name, DBUS_TYPE_INVALID)) {
        SDL_SetError("Unable to create tray: unable to register status notifier item!");
        CLEANUP2();
        return NULL;
    }
    dbus_driver->dbus->connection_flush(tray_dbus->connection);

    /* Icon mouse event callbacks */
    tray_dbus->l_cb = (SDL_TrayClickCallback)SDL_GetPointerProperty(props, SDL_PROP_TRAY_CREATE_LEFTCLICK_CALLBACK_POINTER, NULL);
    tray_dbus->r_cb = (SDL_TrayClickCallback)SDL_GetPointerProperty(props, SDL_PROP_TRAY_CREATE_RIGHTCLICK_CALLBACK_POINTER, NULL);
    tray_dbus->m_cb = (SDL_TrayClickCallback)SDL_GetPointerProperty(props, SDL_PROP_TRAY_CREATE_MIDDLECLICK_CALLBACK_POINTER, NULL);
    tray_dbus->udata = SDL_GetPointerProperty(props, SDL_PROP_TRAY_CREATE_USERDATA_POINTER, NULL);

    return tray;
}

void DestroyDriver(SDL_TrayDriver *driver)
{
    SDL_free(driver->internal);
    SDL_DBus_Quit();
    SDL_free(driver);
}

void DestroyMenu(SDL_TrayMenu *menu)
{
    SDL_TrayMenuDBus *menu_dbus;

    if (!menu) {
        return;
    }

    menu_dbus = (SDL_TrayMenuDBus *)menu->internal;

    if (menu_dbus->menu) {
        SDL_ListNode *cursor;

        cursor = menu_dbus->menu;
        while (cursor) {
            SDL_MenuItem *item;
            SDL_TrayEntry *entry;
            SDL_TrayEntryDBus *entry_dbus;

            item = cursor->entry;
            entry = item->udata;
            entry_dbus = (SDL_TrayEntryDBus *)entry->internal;

            if (entry_dbus->sub_menu) {
                DestroyMenu(entry_dbus->sub_menu);
                entry_dbus->sub_menu = NULL;
            }
            SDL_free((char *)item->utf8);
            SDL_free(entry_dbus);
            SDL_free(item);
            SDL_free(entry);

            cursor = cursor->next;
        }
        SDL_ListClear(&menu_dbus->menu);
    }

    SDL_free(menu_dbus->array_representation);

    SDL_free(menu_dbus);
    SDL_free(menu);
}

void DestroyTray(SDL_Tray *tray)
{
    SDL_TrayDBus *tray_dbus;
    SDL_TrayDriverDBus *driver;

    driver = (SDL_TrayDriverDBus *)tray->driver->internal;
    tray_dbus = (SDL_TrayDBus *)tray->internal;

    driver->dbus->connection_remove_filter(tray_dbus->connection, TrayWatcherOwnerChanged, tray_dbus);

    /* Destroy connection */
    driver->dbus->connection_flush(tray_dbus->connection);
    tray_dbus->block = true;
    driver->dbus->connection_close(tray_dbus->connection);
    driver->dbus->connection_unref(tray_dbus->connection);
    tray_dbus->connection = NULL;

    /* Destroy icon and tooltip */
    SDL_free(tray_dbus->activation_token);
    SDL_free(tray_dbus->tooltip);
    SDL_DestroySurface(tray_dbus->surface);

    /* Destroy the menus and entries */
    if (tray->menu) {
        DestroyMenu(tray->menu);
        tray->menu = NULL;
    }

    SDL_free(tray_dbus->service_name);
    SDL_free(tray_dbus);
    /* Free the tray */
    SDL_free(tray);
}

void UpdateTray(SDL_Tray *tray)
{
    SDL_TrayDBus *tray_dbus;
    SDL_TrayDriverDBus *driver;

    driver = (SDL_TrayDriverDBus *)tray->driver->internal;
    tray_dbus = (SDL_TrayDBus *)tray->internal;

    if (!SDL_ObjectValid(tray, SDL_OBJECT_TYPE_TRAY)) {
        return;
    }

    if (tray_dbus->block) {
        return;
    }

    driver->dbus->connection_read_write(tray_dbus->connection, 0);
    while (SDL_DBus_ConnectionDispatch(driver->dbus, tray_dbus->connection) == DBUS_DISPATCH_DATA_REMAINS) {
        if (!SDL_ObjectValid(tray, SDL_OBJECT_TYPE_TRAY)) {
            break;
        }

        if (tray_dbus->block) {
            break;
        }

        SDL_DelayNS(SDL_US_TO_NS(10));
    }

    if (SDL_ObjectValid(tray, SDL_OBJECT_TYPE_TRAY) && tray_dbus->watcher_needs_register &&
        SDL_GetTicksNS() >= tray_dbus->watcher_retry_time) {
        dbus_bool_t host_registered = FALSE;
        if (SDL_DBus_QueryPropertyOnConnection(tray_dbus->connection, NULL, SNI_WATCHER_SERVICE,
                                               SNI_WATCHER_PATH, SNI_WATCHER_INTERFACE,
                                               "IsStatusNotifierHostRegistered", DBUS_TYPE_BOOLEAN,
                                               &host_registered) && host_registered) {
            const char *register_name = tray_dbus->service_name;
            if (SDL_DBus_CallVoidMethodOnConnection(tray_dbus->connection, SNI_WATCHER_SERVICE,
                                                    SNI_WATCHER_PATH, SNI_WATCHER_INTERFACE,
                                                    "RegisterStatusNotifierItem", DBUS_TYPE_STRING,
                                                    &register_name, DBUS_TYPE_INVALID)) {
                tray_dbus->watcher_needs_register = false;
                driver->dbus->connection_flush(tray_dbus->connection);
            }
        }
        if (tray_dbus->watcher_needs_register) {
            tray_dbus->watcher_retry_time = SDL_GetTicksNS() + SDL_NS_PER_SECOND;
        }
    }
}

void SetTrayIcon(SDL_Tray *tray, SDL_Surface *surface)
{
    SDL_TrayDBus *tray_dbus;
    SDL_TrayDriverDBus *driver;
    DBusMessage *signal;
    SDL_Surface *converted;

    converted = ConvertTrayIcon(surface);
    if (surface && !converted) {
        return;
    }

    driver = (SDL_TrayDriverDBus *)tray->driver->internal;
    tray_dbus = (SDL_TrayDBus *)tray->internal;

    if (tray_dbus->surface) {
        SDL_DestroySurface(tray_dbus->surface);
    }
    tray_dbus->surface = converted;

    signal = driver->dbus->message_new_signal(SNI_OBJECT_PATH, SNI_INTERFACE, "NewIcon");
    if (signal) {
        driver->dbus->connection_send(tray_dbus->connection, signal, NULL);
        driver->dbus->connection_flush(tray_dbus->connection);
        driver->dbus->message_unref(signal);
    }
}

void SetTrayTooltip(SDL_Tray *tray, const char *text)
{
    SDL_TrayDBus *tray_dbus;
    SDL_TrayDriverDBus *driver;
    DBusMessage *signal;
    char *copy;

    driver = (SDL_TrayDriverDBus *)tray->driver->internal;
    tray_dbus = (SDL_TrayDBus *)tray->internal;

    copy = text ? SDL_strdup(text) : NULL;
    if (text && !copy) {
        return;
    }
    SDL_free(tray_dbus->tooltip);
    tray_dbus->tooltip = copy;

    signal = driver->dbus->message_new_signal(SNI_OBJECT_PATH, SNI_INTERFACE, "NewToolTip");
    if (signal) {
        driver->dbus->connection_send(tray_dbus->connection, signal, NULL);
        driver->dbus->connection_flush(tray_dbus->connection);
        driver->dbus->message_unref(signal);
    }
}

SDL_TrayMenu *CreateTrayMenu(SDL_Tray *tray)
{
    SDL_TrayMenuDBus *menu_dbus;

    menu_dbus = SDL_malloc(sizeof(SDL_TrayMenuDBus));
    if (!menu_dbus) {
        SDL_SetError("Unable to create tray menu: allocation failure!");
        return NULL;
    }
    tray->menu = SDL_malloc(sizeof(SDL_TrayMenu));
    if (!tray->menu) {
        SDL_free(menu_dbus);
        SDL_SetError("Unable to create tray menu: allocation failure!");
        return NULL;
    }

    menu_dbus->menu = NULL;
    menu_dbus->menu_path = NULL;
    tray->menu->parent_tray = tray;
    tray->menu->parent_entry = NULL;
    tray->menu->internal = menu_dbus;
    menu_dbus->array_representation = NULL;

    return tray->menu;
}

SDL_TrayMenu *CreateTraySubmenu(SDL_TrayEntry *entry)
{
    SDL_TrayMenuDBus *menu_dbus;
    SDL_TrayMenu *menu;
    SDL_TrayEntryDBus *entry_dbus;

    entry_dbus = (SDL_TrayEntryDBus *)entry->internal;
    menu_dbus = SDL_malloc(sizeof(SDL_TrayMenuDBus));
    if (!menu_dbus) {
        SDL_SetError("Unable to create tray submenu: allocation failure!");
        return NULL;
    }
    menu = SDL_malloc(sizeof(SDL_TrayMenu));
    if (!menu) {
        SDL_free(menu_dbus);
        SDL_SetError("Unable to create tray submenu: allocation failure!");
        return NULL;
    }

    menu_dbus->menu = NULL;
    menu_dbus->menu_path = NULL;
    menu->parent_tray = entry->parent->parent_tray;
    menu->parent_entry = entry;
    menu->internal = menu_dbus;
    entry_dbus->sub_menu = menu;
    menu_dbus->array_representation = NULL;

    return menu;
}

SDL_TrayMenu *GetTraySubmenu(SDL_TrayEntry *entry)
{
    SDL_TrayEntryDBus *entry_dbus;

    entry_dbus = (SDL_TrayEntryDBus *)entry->internal;

    return entry_dbus->sub_menu;
}

bool TrayRightClickHandler(SDL_ListNode *menu, void *udata)
{
    SDL_Tray *tray = (SDL_Tray *)udata;
    SDL_TrayDBus *tray_dbus = (SDL_TrayDBus *)tray->internal;

    char *token = tray_dbus->activation_token;
    tray_dbus->activation_token = NULL;
    bool show_menu = SDL_InvokeTrayClick(tray, tray_dbus->r_cb, tray_dbus->udata, NULL, token);
    SDL_free(token);
    return show_menu;
}

void TraySendNewMenu(SDL_Tray *tray, const char *new_path)
{
    SDL_TrayDriverDBus *driver;
    SDL_TrayDBus *tray_dbus;
    SDL_TrayMenuDBus *menu_dbus;
    DBusMessage *signal;

    tray_dbus = (SDL_TrayDBus *)tray->internal;
    driver = (SDL_TrayDriverDBus *)tray->driver->internal;
    menu_dbus = (SDL_TrayMenuDBus *)tray->menu->internal;

    driver->dbus->connection_flush(tray_dbus->connection);

    signal = driver->dbus->message_new_signal(SNI_OBJECT_PATH, "org.freedesktop.DBus.Properties", "PropertiesChanged");
    if (signal) {
        DBusMessageIter iter, dict, ientry, value;
        const char *iface;
        const char *prop;
        const char *path;
        dbus_bool_t bool_val;

        iface = SNI_INTERFACE;
        prop = "Menu";
        if (new_path) {
            path = menu_dbus->menu_path = new_path;
        } else {
            path = menu_dbus->menu_path;
        }
        bool_val = TrayItemIsMenu(tray, tray_dbus) ? TRUE : FALSE;
        driver->dbus->message_iter_init_append(signal, &iter);
        driver->dbus->message_iter_append_basic(&iter, DBUS_TYPE_STRING, &iface);
        driver->dbus->message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "{sv}", &dict);
        driver->dbus->message_iter_open_container(&dict, DBUS_TYPE_DICT_ENTRY, NULL, &ientry);
        driver->dbus->message_iter_append_basic(&ientry, DBUS_TYPE_STRING, &prop);
        driver->dbus->message_iter_open_container(&ientry, DBUS_TYPE_VARIANT, "o", &value);
        driver->dbus->message_iter_append_basic(&value, DBUS_TYPE_OBJECT_PATH, &path);
        driver->dbus->message_iter_close_container(&ientry, &value);
        driver->dbus->message_iter_close_container(&dict, &ientry);
        prop = "ItemIsMenu";
        driver->dbus->message_iter_open_container(&dict, DBUS_TYPE_DICT_ENTRY, NULL, &ientry);
        driver->dbus->message_iter_append_basic(&ientry, DBUS_TYPE_STRING, &prop);
        driver->dbus->message_iter_open_container(&ientry, DBUS_TYPE_VARIANT, "b", &value);
        driver->dbus->message_iter_append_basic(&value, DBUS_TYPE_BOOLEAN, &bool_val);
        driver->dbus->message_iter_close_container(&ientry, &value);
        driver->dbus->message_iter_close_container(&dict, &ientry);
        driver->dbus->message_iter_close_container(&iter, &dict);
        driver->dbus->message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "s", &dict);
        driver->dbus->message_iter_close_container(&iter, &dict);
        driver->dbus->connection_send(tray_dbus->connection, signal, NULL);
        driver->dbus->connection_flush(tray_dbus->connection);
        driver->dbus->message_unref(signal);
    }

    signal = driver->dbus->message_new_signal(SNI_OBJECT_PATH, SNI_INTERFACE, "NewMenu");
    if (signal) {
        driver->dbus->connection_send(tray_dbus->connection, signal, NULL);
        driver->dbus->connection_flush(tray_dbus->connection);
        driver->dbus->message_unref(signal);
    }

    driver->dbus->connection_flush(tray_dbus->connection);
}

void TrayNewMenuOnMenuUpdateCallback(SDL_ListNode *menu, const char *path, void *cbdata)
{
    TraySendNewMenu((SDL_Tray *)cbdata, path);
}

SDL_TrayEntry *InsertTrayEntryAt(SDL_TrayMenu *menu, int pos, const char *label, SDL_TrayEntryFlags flags)
{
    SDL_Tray *tray;
    SDL_TrayDriverDBus *driver;
    SDL_TrayMenuDBus *menu_dbus;
    SDL_TrayDBus *tray_dbus;
    SDL_TrayEntry *entry;
    SDL_TrayEntryDBus *entry_dbus;
    bool update;

    tray = menu->parent_tray;
    menu_dbus = (SDL_TrayMenuDBus *)menu->internal;
    tray_dbus = (SDL_TrayDBus *)tray->internal;
    driver = (SDL_TrayDriverDBus *)tray->driver->internal;

    entry_dbus = SDL_malloc(sizeof(SDL_TrayEntryDBus));
    if (!entry_dbus) {
        SDL_SetError("Unable to create tray entry: allocation failure!");
        return NULL;
    }
    entry = SDL_malloc(sizeof(SDL_TrayEntry));
    if (!entry) {
        SDL_free(entry_dbus);
        SDL_SetError("Unable to create tray entry: allocation failure!");
        return NULL;
    }

    entry->parent = menu;
    entry->internal = entry_dbus;
    entry_dbus->item = SDL_DBus_CreateMenuItem();
    if (!entry_dbus->item) {
        SDL_free(entry);
        SDL_free(entry_dbus);
        return NULL;
    }
    entry_dbus->item->utf8 = label ? SDL_strdup(label) : NULL;
    if (label && !entry_dbus->item->utf8) {
        SDL_OutOfMemory();
        SDL_free(entry_dbus->item);
        SDL_free(entry);
        SDL_free(entry_dbus);
        return NULL;
    }
    if (!label) {
        entry_dbus->item->type = SDL_MENU_ITEM_TYPE_SEPERATOR;
    } else if (flags & SDL_TRAYENTRY_CHECKBOX) {
        entry_dbus->item->type = SDL_MENU_ITEM_TYPE_CHECKBOX;
    } else {
        entry_dbus->item->type = SDL_MENU_ITEM_TYPE_NORMAL;
    }
    entry_dbus->item->flags = SDL_MENU_ITEM_FLAGS_NONE;
    if (flags & SDL_TRAYENTRY_DISABLED) {
        entry_dbus->item->flags |= SDL_MENU_ITEM_FLAGS_DISABLED;
    }
    if (flags & SDL_TRAYENTRY_CHECKED) {
        entry_dbus->item->flags |= SDL_MENU_ITEM_FLAGS_CHECKED;
    }
    entry_dbus->item->cb_data = NULL;
    entry_dbus->item->cb = NULL;
    entry_dbus->item->sub_menu = NULL;
    entry_dbus->item->udata = entry;
    entry_dbus->sub_menu = NULL;

    if (menu_dbus->menu) {
        update = true;
    } else {
        update = false;
    }

    if (menu->parent_entry) {
        update = true;
    }

    SDL_ListInsertAtPosition(&menu_dbus->menu, pos, entry_dbus->item);

    if (menu->parent_entry) {
        SDL_TrayEntryDBus *parent_entry_dbus;

        parent_entry_dbus = (SDL_TrayEntryDBus *)menu->parent_entry->internal;
        parent_entry_dbus->item->sub_menu = menu_dbus->menu;
    }

    if (update) {
        SDL_TrayMenuDBus *main_menu_dbus;

        main_menu_dbus = (SDL_TrayMenuDBus *)tray->menu->internal;
        SDL_DBus_UpdateMenu(driver->dbus, tray_dbus->connection, main_menu_dbus->menu, main_menu_dbus->menu_path, TrayNewMenuOnMenuUpdateCallback, tray, SDL_DBUS_UPDATE_MENU_FLAGS_NONE);
    } else {
        menu_dbus->menu_path = SDL_DBus_ExportMenu(driver->dbus, tray_dbus->connection, menu_dbus->menu);

        if (menu_dbus->menu_path) {
            TraySendNewMenu(tray, NULL);
        }
    }

    if (menu->parent_tray && !menu->parent_entry && tray_dbus->r_cb) {
        SDL_DBus_RegisterMenuOpenCallback(menu_dbus->menu, TrayRightClickHandler, tray);
    }

    return entry;
}

SDL_TrayEntry **GetTrayEntries(SDL_TrayMenu *menu, int *count)
{
    SDL_TrayEntry **array_representation;
    SDL_TrayMenuDBus *menu_dbus;
    SDL_ListNode *cursor;
    int sz;
    int i;

    menu_dbus = (SDL_TrayMenuDBus *)menu->internal;

    SDL_free(menu_dbus->array_representation);

    sz = SDL_ListCountEntries(&menu_dbus->menu);
    array_representation = SDL_calloc(sz + 1, sizeof(SDL_TrayEntry *));
    if (!array_representation) {
        SDL_SetError("Memory allocation failure!");
        return NULL;
    }

    i = 0;
    cursor = menu_dbus->menu;
    while (cursor) {
        SDL_MenuItem *item;

        item = cursor->entry;
        array_representation[i] = item->udata;
        cursor = cursor->next;
        i++;
    }
    array_representation[sz] = NULL;

    *count = sz;
    return array_representation;
}

void RemoveTrayEntry(SDL_TrayEntry *entry)
{
    SDL_TrayEntryDBus *entry_dbus;
    SDL_TrayMenuDBus *menu_dbus;
    SDL_Tray *tray;
    SDL_TrayDriverDBus *driver;
    SDL_TrayDBus *tray_dbus;
    SDL_TrayMenuDBus *main_menu_dbus;
    const char *old_path;

    tray = entry->parent->parent_tray;
    tray_dbus = (SDL_TrayDBus *)tray->internal;
    driver = (SDL_TrayDriverDBus *)tray->driver->internal;
    entry_dbus = (SDL_TrayEntryDBus *)entry->internal;
    menu_dbus = (SDL_TrayMenuDBus *)entry->parent->internal;
    main_menu_dbus = (SDL_TrayMenuDBus *)tray->menu->internal;

    tray_dbus->block = true;
    if (menu_dbus->menu->entry == entry_dbus->item && menu_dbus->menu->next) {
        SDL_DBus_TransferMenuItemProperties(entry_dbus->item, (SDL_MenuItem *)menu_dbus->menu->next->entry);
    }

    old_path = NULL;
    if (!main_menu_dbus->menu->next) {
        old_path = main_menu_dbus->menu_path;
    }

    driver->dbus->connection_flush(tray_dbus->connection);
    DestroyMenu(entry_dbus->sub_menu);
    SDL_ListRemove(&menu_dbus->menu, entry_dbus->item);
    if (entry->parent->parent_entry) {
        SDL_TrayEntryDBus *parent_entry_dbus = (SDL_TrayEntryDBus *)entry->parent->parent_entry->internal;
        parent_entry_dbus->item->sub_menu = menu_dbus->menu;
    }
    SDL_free((char *)entry_dbus->item->utf8);
    SDL_free(entry_dbus->item);
    SDL_free(entry_dbus);
    SDL_free(entry);

    if (old_path) {
        SDL_DBus_RetractMenu(driver->dbus, tray_dbus->connection, &main_menu_dbus->menu_path);
    }
    SDL_DBus_UpdateMenu(driver->dbus, tray_dbus->connection, main_menu_dbus->menu, main_menu_dbus->menu_path, TrayNewMenuOnMenuUpdateCallback, tray, SDL_DBUS_UPDATE_MENU_FLAGS_NONE);
    driver->dbus->connection_flush(tray_dbus->connection);
    tray_dbus->block = false;
}

void EntryCallback(SDL_MenuItem *item, void *udata)
{
    SDL_TrayCallback entry_cb;

    entry_cb = item->udata2;
    if (entry_cb) {
        entry_cb(udata, item->udata);
    }
}

void SetTrayEntryCallback(SDL_TrayEntry *entry, SDL_TrayCallback callback, void *userdata)
{
    SDL_TrayEntryDBus *entry_dbus;
    SDL_Tray *tray;
    SDL_TrayDriverDBus *driver;
    SDL_TrayDBus *tray_dbus;
    SDL_TrayMenuDBus *main_menu_dbus;

    tray = entry->parent->parent_tray;
    tray_dbus = (SDL_TrayDBus *)tray->internal;
    driver = (SDL_TrayDriverDBus *)tray->driver->internal;
    entry_dbus = (SDL_TrayEntryDBus *)entry->internal;
    main_menu_dbus = (SDL_TrayMenuDBus *)tray->menu->internal;

    entry_dbus->item->cb = EntryCallback;
    entry_dbus->item->cb_data = userdata;
    entry_dbus->item->udata2 = callback;

    SDL_DBus_UpdateMenu(driver->dbus, tray_dbus->connection, main_menu_dbus->menu, main_menu_dbus->menu_path, TrayNewMenuOnMenuUpdateCallback, tray, SDL_DBUS_UPDATE_MENU_FLAGS_NONE);
}

void SetTrayEntryLabel(SDL_TrayEntry *entry, const char *label)
{
    SDL_TrayEntryDBus *entry_dbus;
    SDL_Tray *tray;
    SDL_TrayDriverDBus *driver;
    SDL_TrayDBus *tray_dbus;
    SDL_TrayMenuDBus *main_menu_dbus;

    tray = entry->parent->parent_tray;
    tray_dbus = (SDL_TrayDBus *)tray->internal;
    driver = (SDL_TrayDriverDBus *)tray->driver->internal;
    entry_dbus = (SDL_TrayEntryDBus *)entry->internal;
    main_menu_dbus = (SDL_TrayMenuDBus *)tray->menu->internal;

    if ((label == NULL) != (entry_dbus->item->utf8 == NULL) || !label) {
        return;
    }

    char *copy = SDL_strdup(label);
    if (!copy) {
        return;
    }
    SDL_free((char *)entry_dbus->item->utf8);
    entry_dbus->item->utf8 = copy;

    SDL_DBus_UpdateMenu(driver->dbus, tray_dbus->connection, main_menu_dbus->menu, main_menu_dbus->menu_path, TrayNewMenuOnMenuUpdateCallback, tray, SDL_DBUS_UPDATE_MENU_FLAGS_NONE);
}

void SetTrayEntryShortcut(SDL_TrayEntry *entry, SDL_Scancode scancode, SDL_Keymod mod)
{
    const SDL_Keymod supported_modifiers = SDL_KMOD_CTRL | SDL_KMOD_ALT | SDL_KMOD_SHIFT | SDL_KMOD_GUI;
    SDL_TrayEntryDBus *entry_dbus;
    SDL_Tray *tray;
    SDL_TrayDriverDBus *driver;
    SDL_TrayDBus *tray_dbus;
    SDL_TrayMenuDBus *main_menu_dbus;

    if (!entry) {
        SDL_InvalidParamError("entry");
        return;
    }
    if ((scancode != SDL_SCANCODE_UNKNOWN && ((int)scancode < 0 || scancode >= SDL_SCANCODE_COUNT)) ||
        (mod & ~supported_modifiers) || (scancode == SDL_SCANCODE_UNKNOWN && mod != SDL_KMOD_NONE)) {
        SDL_InvalidParamError("shortcut");
        return;
    }
    if (scancode != SDL_SCANCODE_UNKNOWN && !SDL_GetScancodeName(scancode)[0]) {
        SDL_InvalidParamError("scancode");
        return;
    }

    tray = entry->parent->parent_tray;
    tray_dbus = (SDL_TrayDBus *)tray->internal;
    driver = (SDL_TrayDriverDBus *)tray->driver->internal;
    entry_dbus = (SDL_TrayEntryDBus *)entry->internal;
    main_menu_dbus = (SDL_TrayMenuDBus *)tray->menu->internal;

    entry_dbus->item->shortcut_scancode = scancode;
    entry_dbus->item->shortcut_modifiers = mod;
    SDL_DBus_UpdateMenu(driver->dbus, tray_dbus->connection, main_menu_dbus->menu, main_menu_dbus->menu_path, TrayNewMenuOnMenuUpdateCallback, tray, SDL_DBUS_UPDATE_MENU_FLAGS_NONE);
}

const char *GetTrayEntryLabel(SDL_TrayEntry *entry)
{
    SDL_MenuItem *item = ((SDL_TrayEntryDBus *)entry->internal)->item;
    return item->type == SDL_MENU_ITEM_TYPE_SEPERATOR ? NULL : item->utf8;
}

void SetTrayEntryChecked(SDL_TrayEntry *entry, bool val)
{
    SDL_TrayEntryDBus *entry_dbus;
    SDL_Tray *tray;
    SDL_TrayDriverDBus *driver;
    SDL_TrayDBus *tray_dbus;
    SDL_TrayMenuDBus *main_menu_dbus;

    tray = entry->parent->parent_tray;
    tray_dbus = (SDL_TrayDBus *)tray->internal;
    driver = (SDL_TrayDriverDBus *)tray->driver->internal;
    entry_dbus = (SDL_TrayEntryDBus *)entry->internal;
    main_menu_dbus = (SDL_TrayMenuDBus *)tray->menu->internal;

    if (val) {
        entry_dbus->item->flags |= SDL_MENU_ITEM_FLAGS_CHECKED;
    } else {
        entry_dbus->item->flags &= ~(SDL_MENU_ITEM_FLAGS_CHECKED);
    }

    SDL_DBus_UpdateMenu(driver->dbus, tray_dbus->connection, main_menu_dbus->menu, main_menu_dbus->menu_path, TrayNewMenuOnMenuUpdateCallback, tray, SDL_DBUS_UPDATE_MENU_FLAGS_NONE);
}

bool GetTrayEntryChecked(SDL_TrayEntry *entry)
{
    return ((SDL_TrayEntryDBus *)entry->internal)->item->flags & SDL_MENU_ITEM_FLAGS_CHECKED;
}

void SetTrayEntryEnabled(SDL_TrayEntry *entry, bool val)
{
    SDL_TrayEntryDBus *entry_dbus;
    SDL_Tray *tray;
    SDL_TrayDriverDBus *driver;
    SDL_TrayDBus *tray_dbus;
    SDL_TrayMenuDBus *main_menu_dbus;

    tray = entry->parent->parent_tray;
    tray_dbus = (SDL_TrayDBus *)tray->internal;
    driver = (SDL_TrayDriverDBus *)tray->driver->internal;
    entry_dbus = (SDL_TrayEntryDBus *)entry->internal;
    main_menu_dbus = (SDL_TrayMenuDBus *)tray->menu->internal;

    if (!val) {
        entry_dbus->item->flags |= SDL_MENU_ITEM_FLAGS_DISABLED;
    } else {
        entry_dbus->item->flags &= ~(SDL_MENU_ITEM_FLAGS_DISABLED);
    }

    SDL_DBus_UpdateMenu(driver->dbus, tray_dbus->connection, main_menu_dbus->menu, main_menu_dbus->menu_path, TrayNewMenuOnMenuUpdateCallback, tray, SDL_DBUS_UPDATE_MENU_FLAGS_NONE);
}

bool GetTrayEntryEnabled(SDL_TrayEntry *entry)
{
    return !(((SDL_TrayEntryDBus *)entry->internal)->item->flags & SDL_MENU_ITEM_FLAGS_DISABLED);
}

void ClickTrayEntry(SDL_TrayEntry *entry)
{
    SDL_TrayEntryDBus *dbus_entry;
    SDL_TrayCallback entry_cb;

    dbus_entry = (SDL_TrayEntryDBus *)entry->internal;
    if (dbus_entry->item->type == SDL_MENU_ITEM_TYPE_CHECKBOX) {
        SDL_Tray *tray;
        SDL_TrayDriverDBus *driver;
        SDL_TrayDBus *tray_dbus;
        SDL_TrayMenuDBus *main_menu_dbus;

        tray = entry->parent->parent_tray;
        tray_dbus = (SDL_TrayDBus *)tray->internal;
        driver = (SDL_TrayDriverDBus *)tray->driver->internal;
        main_menu_dbus = (SDL_TrayMenuDBus *)tray->menu->internal;

        dbus_entry->item->flags ^= SDL_MENU_ITEM_FLAGS_CHECKED;
        SDL_DBus_UpdateMenu(driver->dbus, tray_dbus->connection, main_menu_dbus->menu, main_menu_dbus->menu_path, TrayNewMenuOnMenuUpdateCallback, tray, SDL_DBUS_UPDATE_MENU_FLAGS_NONE);
    }
    entry_cb = dbus_entry->item->cb ? dbus_entry->item->udata2 : NULL;
    if (entry_cb) {
        entry_cb(dbus_entry->item->cb_data, dbus_entry->item->udata);
    }
}

SDL_TrayDriver *SDL_Tray_CreateDBusDriver(void)
{
    SDL_TrayDriverDBus *dbus_driver;
    SDL_TrayDriver *driver;
    SDL_DBusContext *ctx;
    DBusMessage *saved;
    char **paths;
    int count;
    int i;
    bool sni_supported;

    /* Init DBus and get context */
    SDL_DBus_Init();
    ctx = SDL_DBus_GetContext();
    if (!ctx) {
        return NULL;
    }

    /* SNI support detection */
    sni_supported = false;
    paths = NULL;
    count = 0;

    if (SDL_DBus_CallMethod(&saved, "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "ListNames", DBUS_TYPE_INVALID, DBUS_TYPE_ARRAY, DBUS_TYPE_STRING, &paths, &count, DBUS_TYPE_INVALID)) {
        SDL_DBus_FreeReply(&saved);

        if (paths) {
            bool watcher_found;

            watcher_found = false;
            for (i = 0; i < count; i++) {
                if (!SDL_strcmp(paths[i], SNI_WATCHER_SERVICE)) {
                    watcher_found = true;
                }
            }
            ctx->free_string_array(paths);

            if (watcher_found) {
                dbus_bool_t host_registered;

                host_registered = FALSE;
                SDL_DBus_QueryProperty(&saved, SNI_WATCHER_SERVICE, SNI_WATCHER_PATH, SNI_WATCHER_INTERFACE, "IsStatusNotifierHostRegistered", DBUS_TYPE_BOOLEAN, &host_registered);
                SDL_DBus_FreeReply(&saved);
                if (host_registered) {
                    sni_supported = true;
                }
            }
        }
    }

    if (!sni_supported) {
        SDL_SetError("Unable to create tray: no SNI support!");
        SDL_DBus_Quit();
        return NULL;
    }

    /* Allocate the driver struct */
    dbus_driver = SDL_malloc(sizeof(SDL_TrayDriverDBus));
    if (!dbus_driver) {
        SDL_DBus_Quit();
        return NULL;
    }
    driver = SDL_malloc(sizeof(SDL_TrayDriver));
    if (!driver) {
        SDL_free(dbus_driver);
        SDL_DBus_Quit();
        return NULL;
    }

    /* Populate */
    dbus_driver->dbus = ctx;
    driver->internal = dbus_driver;
    driver->name = "dbus";
    driver->count = 0;
    driver->CreateTray = CreateTray;
    driver->DestroyTray = DestroyTray;
    driver->UpdateTray = UpdateTray;
    driver->SetTrayIcon = SetTrayIcon;
    driver->SetTrayTooltip = SetTrayTooltip;
    driver->CreateTrayMenu = CreateTrayMenu;
    driver->InsertTrayEntryAt = InsertTrayEntryAt;
    driver->CreateTraySubmenu = CreateTraySubmenu;
    driver->GetTraySubmenu = GetTraySubmenu;
    driver->GetTrayEntries = GetTrayEntries;
    driver->RemoveTrayEntry = RemoveTrayEntry;
    driver->SetTrayEntryCallback = SetTrayEntryCallback;
    driver->SetTrayEntryLabel = SetTrayEntryLabel;
    driver->SetTrayEntryShortcut = SetTrayEntryShortcut;
    driver->GetTrayEntryLabel = GetTrayEntryLabel;
    driver->SetTrayEntryChecked = SetTrayEntryChecked;
    driver->GetTrayEntryChecked = GetTrayEntryChecked;
    driver->SetTrayEntryEnabled = SetTrayEntryEnabled;
    driver->GetTrayEntryEnabled = GetTrayEntryEnabled;
    driver->ClickTrayEntry = ClickTrayEntry;
    driver->DestroyDriver = DestroyDriver;

    return driver;
}

#endif // SDL_USE_LIBDBUS
