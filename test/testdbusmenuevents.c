/* Regression: callbacks may destroy or replace a menu during EventGroup. */
#include "../src/core/linux/SDL_dbus.c"
#include "../src/tray/unix/SDL_dbustray.c"
#undef NDEBUG
#include <assert.h>

static SDL_ListNode *registered_menu;
static int replies, calls, refs, connection_closes;
static bool connected, destroy;
static bool destroy_tray_in_click;
static bool expect_open_connection_reply;
static bool expect_error;
static const char *expected_error_name;
static bool click_show_menu = true;
static bool expect_getall_reply;
static bool expect_window_id_reply;
static bool expect_menu_reply;
static const char *expected_menu_path;
static SDL_Tray *expected_tray;
static int expected_click;

static void assert_tray_click_context(SDL_Tray *tray)
{
    int x, y;
    assert(tray == expected_tray);
    assert(SDL_GetTrayClickPosition(tray, &x, &y));
    assert(x == -12 && y == 34);
    assert(SDL_strcmp(SDL_GetTrayClickActivationToken(tray), "test-token") == 0);
}

static bool SDLCALL tray_left_click(void *userdata, SDL_Tray *tray)
{
    assert_tray_click_context(tray);
    expected_click = 1;
    if (destroy_tray_in_click) {
        SDL_UnregisterTray(tray);
        DestroyTray(tray);
    }
    return click_show_menu;
}

static bool SDLCALL tray_right_click(void *userdata, SDL_Tray *tray)
{
    assert_tray_click_context(tray);
    expected_click = 2;
    return true;
}

static bool SDLCALL tray_middle_click(void *userdata, SDL_Tray *tray)
{
    assert_tray_click_context(tray);
    expected_click = 3;
    return true;
}

const char *SDL_GetAppID(void)
{
    return "test";
}

static void test_tray_icon_byte_order(void)
{
    const Uint8 rgba[] = { 0x11, 0x22, 0x33, 0x44 };
    const Uint8 argb[] = { 0x44, 0x11, 0x22, 0x33 };
    SDL_Surface *source = SDL_CreateSurface(1, 1, SDL_PIXELFORMAT_RGBA32);
    assert(source);
    assert(SDL_LockSurface(source));
    SDL_memcpy(source->pixels, rgba, sizeof(rgba));
    SDL_UnlockSurface(source);

    SDL_Surface *wire = ConvertTrayIcon(source);
    assert(wire);
    assert(SDL_LockSurface(wire));
    assert(SDL_memcmp(wire->pixels, argb, sizeof(argb)) == 0);
    SDL_UnlockSurface(wire);
    SDL_DestroySurface(wire);
    SDL_DestroySurface(source);
}

static void test_tray_entry_without_callback(void)
{
    SDL_MenuItem item = { 0 };
    EntryCallback(&item, NULL);
}

static void SDLCALL count_tray_entry_callback(void *userdata, SDL_TrayEntry *entry)
{
    (void)entry;
    ++*(int *)userdata;
}

static void test_disabled_tray_entry_cannot_be_clicked(void)
{
    SDL_TrayEntry entry = { 0 };
    SDL_TrayEntryDBus entry_dbus = { 0 };
    SDL_MenuItem item = { 0 };
    int callback_count = 0;
    entry.internal = &entry_dbus;
    entry_dbus.item = &item;
    item.flags = SDL_MENU_ITEM_FLAGS_DISABLED;
    item.cb = EntryCallback;
    item.cb_data = &callback_count;
    item.udata = &entry;
    item.udata2 = count_tray_entry_callback;

    ClickTrayEntry(&entry);
    assert(callback_count == 0);
}

static void test_tray_entries_cache(void)
{
    SDL_TrayMenu menu = { 0 };
    SDL_TrayMenuDBus menu_dbus = { 0 };
    SDL_TrayEntry entry = { 0 };
    SDL_MenuItem item = { 0 };
    int count = -1;
    menu.internal = &menu_dbus;
    item.udata = &entry;
    assert(SDL_ListAppend(&menu_dbus.menu, &item));

    SDL_TrayEntry **entries = GetTrayEntries(&menu, &count);
    assert(entries && count == 1 && entries[0] == &entry && entries[1] == NULL);
    assert(GetTrayEntries(&menu, &count) == entries);
    assert(count == 1);
    assert(GetTrayEntries(&menu, NULL) == entries);

    SDL_free(menu_dbus.array_representation);
    SDL_ListClear(&menu_dbus.menu);
}

static void test_tray_menu_creation_guards(void)
{
    SDL_Tray tray = { 0 };
    SDL_TrayMenu *menu = CreateTrayMenu(&tray);
    assert(menu && tray.menu == menu);
    assert(!CreateTrayMenu(&tray));
    DestroyMenu(menu);
    tray.menu = NULL;

    SDL_TrayMenu parent_menu = { 0 };
    SDL_TrayEntry entry = { 0 };
    SDL_TrayEntryDBus entry_dbus = { 0 };
    parent_menu.parent_tray = &tray;
    entry.parent = &parent_menu;
    entry.internal = &entry_dbus;

    entry_dbus.flags = SDL_TRAYENTRY_BUTTON;
    assert(!CreateTraySubmenu(&entry));
    entry_dbus.flags = SDL_TRAYENTRY_SUBMENU;
    SDL_TrayMenu *submenu = CreateTraySubmenu(&entry);
    assert(submenu && entry_dbus.sub_menu == submenu);
    assert(!CreateTraySubmenu(&entry));
    DestroyMenu(submenu);
    entry_dbus.sub_menu = NULL;
}

static void test_tray_insert_rejects_out_of_range_position(void)
{
    SDL_TrayMenu menu = { 0 };
    SDL_TrayMenuDBus menu_dbus = { 0 };
    menu.internal = &menu_dbus;

    assert(!InsertTrayEntryAt(&menu, -2, "Invalid", SDL_TRAYENTRY_BUTTON));
    assert(!InsertTrayEntryAt(&menu, 1, "Invalid", SDL_TRAYENTRY_BUTTON));
    assert(!InsertTrayEntryAt(&menu, 0, "Invalid", 0));
    assert(!menu_dbus.menu);
}

static void retain_connection(DBusConnection *conn) { ++refs; }
static void release_connection(DBusConnection *conn) { --refs; }
static void close_connection(DBusConnection *conn) { ++connection_closes; }
static void flush_connection(DBusConnection *conn) {}
static dbus_bool_t remove_filter(DBusConnection *conn, DBusHandleMessageFunction handler, void *data) { return true; }
static dbus_bool_t is_connected(DBusConnection *conn) { return connected; }
static dbus_bool_t get_menu(DBusConnection *conn, const char *path, void **data)
{
    assert(SDL_strcmp(path, DBUS_MENU_OBJECT_PATH) == 0);
    *data = registered_menu;
    return true;
}
static dbus_bool_t send_reply(DBusConnection *conn, DBusMessage *msg, dbus_uint32_t *serial)
{
    assert(calls == 0);
    if (expect_open_connection_reply) {
        assert(connection_closes == 0 && refs == 2);
        expect_open_connection_reply = false;
    }
    assert(dbus_message_get_type(msg) == (expect_error ? DBUS_MESSAGE_TYPE_ERROR : DBUS_MESSAGE_TYPE_METHOD_RETURN));
    if (expect_error) {
        assert(SDL_strcmp(dbus_message_get_error_name(msg), expected_error_name ? expected_error_name : DBUS_ERROR_INVALID_ARGS) == 0);
    }
    if (expect_getall_reply) {
        DBusMessageIter iter, entries;
        bool found_icon = false, found_tooltip = false, found_window_id = false;
        assert(SDL_strcmp(dbus_message_get_signature(msg), "a{sv}") == 0);
        assert(dbus_message_iter_init(msg, &iter));
        assert(dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_ARRAY);
        dbus_message_iter_recurse(&iter, &entries);
        while (dbus_message_iter_get_arg_type(&entries) == DBUS_TYPE_DICT_ENTRY) {
            DBusMessageIter entry, value;
            const char *name;
            dbus_message_iter_recurse(&entries, &entry);
            dbus_message_iter_get_basic(&entry, &name);
            assert(dbus_message_iter_next(&entry));
            dbus_message_iter_recurse(&entry, &value);
            if (!SDL_strcmp(name, "IconPixmap")) {
                found_icon = true;
                assert(dbus_message_iter_get_arg_type(&value) == DBUS_TYPE_ARRAY);
            } else if (!SDL_strcmp(name, "ToolTip")) {
                found_tooltip = true;
                assert(dbus_message_iter_get_arg_type(&value) == DBUS_TYPE_STRUCT);
            } else if (!SDL_strcmp(name, "WindowId")) {
                found_window_id = true;
                assert(dbus_message_iter_get_arg_type(&value) == DBUS_TYPE_UINT32);
            }
            dbus_message_iter_next(&entries);
        }
        assert(found_icon && found_tooltip && found_window_id);
    }
    if (expect_window_id_reply) {
        DBusMessageIter iter, value;
        dbus_uint32_t window_id;
        assert(SDL_strcmp(dbus_message_get_signature(msg), "v") == 0);
        assert(dbus_message_iter_init(msg, &iter));
        assert(dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_VARIANT);
        dbus_message_iter_recurse(&iter, &value);
        assert(dbus_message_iter_get_arg_type(&value) == DBUS_TYPE_UINT32);
        dbus_message_iter_get_basic(&value, &window_id);
        assert(window_id == 0);
    }
    if (expect_menu_reply) {
        DBusMessageIter iter, value;
        const char *menu_path;
        assert(SDL_strcmp(dbus_message_get_signature(msg), "v") == 0);
        assert(dbus_message_iter_init(msg, &iter));
        assert(dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_VARIANT);
        dbus_message_iter_recurse(&iter, &value);
        assert(dbus_message_iter_get_arg_type(&value) == DBUS_TYPE_OBJECT_PATH);
        dbus_message_iter_get_basic(&value, &menu_path);
        assert(SDL_strcmp(menu_path, expected_menu_path) == 0);
    }
    ++replies;
    return true;
}

static void test_tray_getall_without_icon_or_tooltip(SDL_DBusContext *ctx)
{
    SDL_Tray tray = { 0 };
    SDL_TrayDBus tray_dbus = { 0 };
    SDL_TrayDriverDBus driver = { 0 };
    DBusMessage *msg = dbus_message_new_method_call("org.test.Tray", SNI_OBJECT_PATH, "org.freedesktop.DBus.Properties", "GetAll");
    const char *interface = SNI_INTERFACE;

    assert(msg);
    dbus_message_set_serial(msg, 1);
    driver.dbus = ctx;
    tray.internal = &tray_dbus;
    tray_dbus.connection = (DBusConnection *)ctx;
    assert(dbus_message_append_args(msg, DBUS_TYPE_STRING, &interface, DBUS_TYPE_INVALID));
    replies = calls = 0;
    expect_error = false;
    expect_getall_reply = true;
    assert(TrayHandleGetAllProps(&tray, &tray_dbus, &driver, msg) == DBUS_HANDLER_RESULT_HANDLED);
    expect_getall_reply = false;
    assert(replies == 1);
    dbus_message_unref(msg);
}

static void test_tray_get_window_id(SDL_DBusContext *ctx)
{
    SDL_Tray tray = { 0 };
    SDL_TrayDBus tray_dbus = { 0 };
    SDL_TrayDriverDBus driver = { 0 };
    DBusMessage *msg = dbus_message_new_method_call("org.test.Tray", SNI_OBJECT_PATH, "org.freedesktop.DBus.Properties", "Get");
    const char *interface = SNI_INTERFACE;
    const char *property = "WindowId";

    assert(msg);
    dbus_message_set_serial(msg, 1);
    driver.dbus = ctx;
    tray.internal = &tray_dbus;
    tray_dbus.connection = (DBusConnection *)ctx;
    assert(dbus_message_append_args(msg, DBUS_TYPE_STRING, &interface, DBUS_TYPE_STRING, &property, DBUS_TYPE_INVALID));
    replies = calls = 0;
    expect_error = false;
    expect_window_id_reply = true;
    assert(TrayHandleGetProp(&tray, &tray_dbus, &driver, msg) == DBUS_HANDLER_RESULT_HANDLED);
    expect_window_id_reply = false;
    assert(replies == 1);
    dbus_message_unref(msg);
}

static void test_tray_get_menu_path(SDL_DBusContext *ctx)
{
    SDL_Tray tray = { 0 };
    SDL_TrayDBus tray_dbus = { 0 };
    SDL_TrayDriverDBus driver = { 0 };
    SDL_TrayMenu menu = { 0 };
    SDL_TrayMenuDBus menu_dbus = { 0 };
    DBusMessage *msg = dbus_message_new_method_call("org.test.Tray", SNI_OBJECT_PATH, "org.freedesktop.DBus.Properties", "Get");
    const char *interface = SNI_INTERFACE;
    const char *property = "Menu";

    assert(msg);
    dbus_message_set_serial(msg, 1);
    driver.dbus = ctx;
    tray.internal = &tray_dbus;
    tray.menu = &menu;
    tray_dbus.connection = (DBusConnection *)ctx;
    menu.internal = &menu_dbus;
    menu_dbus.menu_path = "/com/canonical/dbusmenu";
    assert(dbus_message_append_args(msg, DBUS_TYPE_STRING, &interface, DBUS_TYPE_STRING, &property, DBUS_TYPE_INVALID));
    replies = calls = 0;
    expect_error = false;
    expect_menu_reply = true;
    expected_menu_path = menu_dbus.menu_path;
    assert(TrayHandleGetProp(&tray, &tray_dbus, &driver, msg) == DBUS_HANDLER_RESULT_HANDLED);
    expect_menu_reply = false;
    expected_menu_path = NULL;
    assert(replies == 1);
    dbus_message_unref(msg);
}

static bool test_tray_click_callback(void *userdata, SDL_Tray *tray)
{
    (void)userdata;
    (void)tray;
    return false;
}

static void test_item_is_menu_matches_left_click_behavior(void)
{
    SDL_Tray tray = { 0 };
    SDL_TrayMenu menu = { 0 };
    SDL_TrayMenuDBus menu_dbus = { 0 };
    SDL_TrayDBus tray_dbus = { 0 };

    tray.menu = &menu;
    menu.internal = &menu_dbus;
    menu_dbus.menu_path = "/MenuBar";
    assert(TrayItemIsMenu(&tray, &tray_dbus));
    tray_dbus.l_cb = test_tray_click_callback;
    assert(!TrayItemIsMenu(&tray, &tray_dbus));
    tray_dbus.l_cb = NULL;
    menu_dbus.menu_path = NULL;
    assert(!TrayItemIsMenu(&tray, &tray_dbus));
}

static void test_tray_watcher_restart_filter(SDL_DBusContext *ctx)
{
    SDL_TrayDBus tray_dbus = { 0 };
    DBusMessage *msg;
    DBusMessageIter iter;
    const char *name = SNI_WATCHER_SERVICE;
    const char *old_owner = ":1.1";
    const char *new_owner = ":1.2";

    tray_dbus.dbus = ctx;
    msg = dbus_message_new_signal("/org/freedesktop/DBus", "org.freedesktop.DBus", "NameOwnerChanged");
    assert(msg);
    dbus_message_iter_init_append(msg, &iter);
    assert(dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &name));
    assert(dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &old_owner));
    assert(dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &new_owner));
    assert(TrayWatcherOwnerChanged(NULL, msg, &tray_dbus) == DBUS_HANDLER_RESULT_NOT_YET_HANDLED);
    assert(tray_dbus.watcher_needs_register && tray_dbus.watcher_retry_time == 0);
    dbus_message_unref(msg);

    old_owner = new_owner;
    new_owner = "";
    msg = dbus_message_new_signal("/org/freedesktop/DBus", "org.freedesktop.DBus", "NameOwnerChanged");
    assert(msg);
    dbus_message_iter_init_append(msg, &iter);
    assert(dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &name));
    assert(dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &old_owner));
    assert(dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &new_owner));
    assert(TrayWatcherOwnerChanged(NULL, msg, &tray_dbus) == DBUS_HANDLER_RESULT_NOT_YET_HANDLED);
    assert(!tray_dbus.watcher_needs_register);
    dbus_message_unref(msg);
}

static void clicked(SDL_MenuItem *item, void *data)
{
    SDL_ListNode *old = registered_menu;
    assert(replies == 1 && refs == 1);
    ++calls;
    registered_menu = old->next;
    SDL_free(old->entry);
    SDL_free(old);
    if (destroy) {
        connected = false;
        SDL_free(registered_menu->entry);
        SDL_free(registered_menu);
        registered_menu = NULL;
    }
}

static void count_menu_callback(SDL_MenuItem *item, void *userdata)
{
    (void)item;
    ++*(int *)userdata;
}

static void test_disabled_menu_items_ignore_events(SDL_DBusContext *ctx)
{
    SDL_ListNode *menu = NULL;
    SDL_DBusMenuItem *item = SDL_calloc(1, sizeof(*item));
    DBusMessage *msg;
    int callback_count = 0;
    assert(item);
    item->id = 1;
    item->_parent.type = SDL_MENU_ITEM_TYPE_CHECKBOX;
    item->_parent.flags = SDL_MENU_ITEM_FLAGS_DISABLED;
    item->_parent.cb = count_menu_callback;
    item->_parent.cb_data = &callback_count;
    assert(SDL_ListAppend(&menu, item));

    msg = dbus_message_new_method_call("org.test.Menu", DBUS_MENU_OBJECT_PATH, DBUS_MENU_INTERFACE, "Event");
    assert(msg);
    dbus_message_set_serial(msg, 1);
    dbus_int32_t id = 1;
    dbus_uint32_t timestamp = 0;
    const char *event_name = "clicked";
    DBusMessageIter iter, variant;
    dbus_message_iter_init_append(msg, &iter);
    assert(dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &id));
    assert(dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &event_name));
    assert(dbus_message_iter_open_container(&iter, DBUS_TYPE_VARIANT, "i", &variant));
    assert(dbus_message_iter_append_basic(&variant, DBUS_TYPE_INT32, &id));
    assert(dbus_message_iter_close_container(&iter, &variant));
    assert(dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &timestamp));
    replies = calls = 0;
    expect_error = false;
    ctx->message_new_signal = dbus_message_new_signal;
    ctx->message_append_args = dbus_message_append_args;
    assert(MenuHandleEvent(ctx, menu, (DBusConnection *)ctx, msg) == DBUS_HANDLER_RESULT_HANDLED);
    assert(replies == 1 && callback_count == 0 && !(item->_parent.flags & SDL_MENU_ITEM_FLAGS_CHECKED));
    dbus_message_unref(msg);

    registered_menu = menu;
    connected = true;
    msg = dbus_message_new_method_call("org.test.Menu", DBUS_MENU_OBJECT_PATH, DBUS_MENU_INTERFACE, "EventGroup");
    assert(msg);
    dbus_message_set_serial(msg, 1);
    dbus_message_iter_init_append(msg, &iter);
    DBusMessageIter array, event;
    assert(dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "(isvu)", &array));
    assert(dbus_message_iter_open_container(&array, DBUS_TYPE_STRUCT, NULL, &event));
    assert(dbus_message_iter_append_basic(&event, DBUS_TYPE_INT32, &id));
    assert(dbus_message_iter_append_basic(&event, DBUS_TYPE_STRING, &event_name));
    assert(dbus_message_iter_open_container(&event, DBUS_TYPE_VARIANT, "i", &variant));
    assert(dbus_message_iter_append_basic(&variant, DBUS_TYPE_INT32, &id));
    assert(dbus_message_iter_close_container(&event, &variant));
    assert(dbus_message_iter_append_basic(&event, DBUS_TYPE_UINT32, &timestamp));
    assert(dbus_message_iter_close_container(&array, &event));
    assert(dbus_message_iter_close_container(&iter, &array));
    replies = calls = refs = 0;
    assert(MenuHandleEventGroup(ctx, menu, (DBusConnection *)ctx, msg) == DBUS_HANDLER_RESULT_HANDLED);
    assert(replies == 1 && callback_count == 0 && refs == 0 && !(item->_parent.flags & SDL_MENU_ITEM_FLAGS_CHECKED));
    dbus_message_unref(msg);
    registered_menu = NULL;
    SDL_ListClear(&menu);
    SDL_free(item);
}

static void test_shortcut_property(SDL_DBusContext *ctx, SDL_Scancode scancode, const char *key_name)
{
    DBusMessage *msg = dbus_message_new_signal(DBUS_MENU_OBJECT_PATH, DBUS_MENU_INTERFACE, "Test");
    DBusMessageIter body, properties;
    SDL_DBusMenuItem item = { 0 };
    assert(msg);
    item._parent.shortcut_scancode = scancode;
    item._parent.shortcut_modifiers = SDL_KMOD_CTRL | SDL_KMOD_SHIFT;
    dbus_message_iter_init_append(msg, &body);
    assert(dbus_message_iter_open_container(&body, DBUS_TYPE_ARRAY, "{sv}", &properties));
    MenuAppendItemProperties(ctx, &item, &properties);
    assert(dbus_message_iter_close_container(&body, &properties));

    DBusMessageIter array, entry, pair, value, sequence, keypress;
    const char *name;
    const char *expected[] = { "Control", "Shift", key_name };
    bool found_shortcut = false;
    int index = 0;
    assert(dbus_message_iter_init(msg, &array));
    dbus_message_iter_recurse(&array, &entry);
    while (dbus_message_iter_get_arg_type(&entry) != DBUS_TYPE_INVALID) {
        dbus_message_iter_recurse(&entry, &pair);
        dbus_message_iter_get_basic(&pair, &name);
        if (SDL_strcmp(name, "shortcut") == 0) {
            found_shortcut = true;
            assert(dbus_message_iter_next(&pair));
            dbus_message_iter_recurse(&pair, &value);
            dbus_message_iter_recurse(&value, &sequence);
            if (scancode == SDL_SCANCODE_UNKNOWN) {
                assert(dbus_message_iter_get_arg_type(&sequence) == DBUS_TYPE_INVALID);
            } else {
                dbus_message_iter_recurse(&sequence, &keypress);
                while (dbus_message_iter_get_arg_type(&keypress) == DBUS_TYPE_STRING) {
                    dbus_message_iter_get_basic(&keypress, &name);
                    assert(index < SDL_arraysize(expected));
                    assert(SDL_strcmp(name, expected[index++]) == 0);
                    dbus_message_iter_next(&keypress);
                }
            }
            break;
        }
        dbus_message_iter_next(&entry);
    }
    assert(found_shortcut);
    assert(index == (scancode == SDL_SCANCODE_UNKNOWN ? 0 : SDL_arraysize(expected)));
    dbus_message_unref(msg);
}

static void test_tray_click_method_routing(SDL_DBusContext *ctx)
{
    SDL_Tray tray = { 0 };
    SDL_TrayDriver tray_driver = { 0 };
    SDL_TrayDBus tray_dbus = { 0 };
    SDL_TrayDriverDBus driver_dbus = { 0 };
    SDL_TrayMenu menu = { 0 };
    SDL_TrayMenuDBus menu_dbus = { 0 };
    const char *methods[] = { "Activate", "ContextMenu", "SecondaryActivate" };

    tray.driver = &tray_driver;
    tray.internal = &tray_dbus;
    tray_driver.internal = &driver_dbus;
    tray_dbus.l_cb = tray_left_click;
    tray_dbus.r_cb = tray_right_click;
    tray_dbus.m_cb = tray_middle_click;
    tray_dbus.activation_token = SDL_strdup("test-token");
    assert(tray_dbus.activation_token);
    tray.menu = &menu;
    menu.internal = &menu_dbus;
    menu_dbus.menu_path = "/MenuBar";
    driver_dbus.dbus = ctx;
    tray_dbus.connection = (DBusConnection *)ctx;

    SDL_RegisterTray(&tray);
    expected_tray = &tray;
    for (int i = 0; i < SDL_arraysize(methods); ++i) {
        if (i > 0) {
            tray_dbus.activation_token = SDL_strdup("test-token");
            assert(tray_dbus.activation_token);
        }
        click_show_menu = true;
        expected_error_name = DBUS_ERROR_FAILED;
        expect_error = (i == 0);
        DBusMessage *msg = dbus_message_new_method_call("org.test.Tray", SNI_OBJECT_PATH, SNI_INTERFACE, methods[i]);
        dbus_int32_t x = -12, y = 34;
        assert(msg);
        dbus_message_set_serial(msg, 1);
        assert(dbus_message_append_args(msg, DBUS_TYPE_INT32, &x, DBUS_TYPE_INT32, &y, DBUS_TYPE_INVALID));
        calls = replies = expected_click = 0;
        assert(TrayMessageHandler((DBusConnection *)ctx, msg, &tray) == DBUS_HANDLER_RESULT_HANDLED);
        assert(replies == 1 && calls == 0 && refs == 0);
        assert(expected_click == i + 1);
        dbus_message_unref(msg);
    }

    click_show_menu = false;
    expect_error = false;
    tray_dbus.activation_token = SDL_strdup("test-token");
    assert(tray_dbus.activation_token);
    DBusMessage *msg = dbus_message_new_method_call("org.test.Tray", SNI_OBJECT_PATH, SNI_INTERFACE, "Activate");
    dbus_int32_t x = -12, y = 34;
    assert(msg);
    dbus_message_set_serial(msg, 1);
    assert(dbus_message_append_args(msg, DBUS_TYPE_INT32, &x, DBUS_TYPE_INT32, &y, DBUS_TYPE_INVALID));
    calls = replies = expected_click = 0;
    assert(TrayMessageHandler((DBusConnection *)ctx, msg, &tray) == DBUS_HANDLER_RESULT_HANDLED);
    assert(replies == 1 && calls == 0 && refs == 0 && expected_click == 1);
    dbus_message_unref(msg);

    SDL_UnregisterTray(&tray);
    SDL_free(tray_dbus.activation_token);
    SDL_Tray *destroyed_tray = SDL_calloc(1, sizeof(*destroyed_tray));
    SDL_TrayDBus *destroyed_tray_dbus = SDL_calloc(1, sizeof(*destroyed_tray_dbus));
    SDL_TrayMenu *destroyed_menu = SDL_calloc(1, sizeof(*destroyed_menu));
    SDL_TrayMenuDBus *destroyed_menu_dbus = SDL_calloc(1, sizeof(*destroyed_menu_dbus));
    assert(destroyed_tray && destroyed_tray_dbus && destroyed_menu && destroyed_menu_dbus);
    destroyed_tray->driver = &tray_driver;
    destroyed_tray->internal = destroyed_tray_dbus;
    destroyed_tray->menu = destroyed_menu;
    destroyed_tray_dbus->connection = (DBusConnection *)ctx;
    destroyed_tray_dbus->l_cb = tray_left_click;
    destroyed_tray_dbus->activation_token = SDL_strdup("test-token");
    assert(destroyed_tray_dbus->activation_token);
    destroyed_menu->internal = destroyed_menu_dbus;
    destroyed_menu_dbus->menu_path = "/MenuBar";
    expected_tray = destroyed_tray;
    SDL_RegisterTray(destroyed_tray);
    click_show_menu = true;
    destroy_tray_in_click = true;
    expect_error = true;
    refs = 1; // The tray owns one connection reference before the callback.
    connection_closes = 0;
    expect_open_connection_reply = true;
    tray_driver.DestroyTray = DestroyTray;
    driver_dbus.dbus = ctx;
    msg = dbus_message_new_method_call("org.test.Tray", SNI_OBJECT_PATH, SNI_INTERFACE, "Activate");
    assert(msg);
    dbus_message_set_serial(msg, 1);
    assert(dbus_message_append_args(msg, DBUS_TYPE_INT32, &x, DBUS_TYPE_INT32, &y, DBUS_TYPE_INVALID));
    calls = replies = expected_click = 0;
    assert(TrayMessageHandler((DBusConnection *)ctx, msg, destroyed_tray) == DBUS_HANDLER_RESULT_HANDLED);
    assert(replies == 1 && calls == 0 && refs == 0 && connection_closes == 1 && expected_click == 1);
    dbus_message_unref(msg);
    destroy_tray_in_click = false;
    expected_error_name = NULL;
    expected_tray = NULL;
}

int main(int argc, char **argv)
{
    SDL_DBusContext ctx = { 0 };
#define USE(name) ctx.name = dbus_##name
    USE(message_is_method_call);
    USE(message_new_method_return); USE(message_iter_init_append);
    USE(message_new_error);
    USE(message_iter_open_container); USE(message_iter_close_container);
    USE(message_unref); USE(message_iter_init); USE(message_iter_get_arg_type);
    USE(message_iter_recurse); USE(message_iter_get_basic); USE(message_iter_next);
    USE(message_iter_append_basic);
    USE(message_is_signal); USE(message_get_args);
#undef USE
    ctx.connection_ref = retain_connection;
    ctx.connection_unref = release_connection;
    ctx.connection_close = close_connection;
    ctx.connection_flush = flush_connection;
    ctx.connection_remove_filter = remove_filter;
    ctx.connection_send = send_reply;
    ctx.connection_get_is_connected = is_connected;
    ctx.connection_get_object_path_data = get_menu;
    test_tray_icon_byte_order();
    test_tray_entry_without_callback();
    test_disabled_tray_entry_cannot_be_clicked();
    test_tray_entries_cache();
    test_tray_menu_creation_guards();
    test_tray_insert_rejects_out_of_range_position();
    test_tray_getall_without_icon_or_tooltip(&ctx);
    test_tray_get_window_id(&ctx);
    test_tray_get_menu_path(&ctx);
    test_item_is_menu_matches_left_click_behavior();
    test_tray_watcher_restart_filter(&ctx);
    test_shortcut_property(&ctx, SDL_SCANCODE_5, "5");
    test_shortcut_property(&ctx, SDL_SCANCODE_KP_1, "KP_1");
    test_shortcut_property(&ctx, SDL_SCANCODE_UNKNOWN, NULL);
    assert(SDL_Init(0));
    test_disabled_menu_items_ignore_events(&ctx);
    test_tray_click_method_routing(&ctx);
    for (int fields = 0; fields <= 5; ++fields) {
        DBusMessage *msg = dbus_message_new_method_call("org.test.Menu", DBUS_MENU_OBJECT_PATH, DBUS_MENU_INTERFACE, "Event");
        assert(msg);
        dbus_message_set_serial(msg, 1);
        DBusMessageIter iter, variant;
        dbus_int32_t id = 1;
        dbus_uint32_t timestamp = 0;
        const char *name = "clicked";
        dbus_message_iter_init_append(msg, &iter);
        if (fields > 0) { assert(dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &id)); }
        if (fields > 1) { assert(dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &name)); }
        if (fields > 2) {
            assert(dbus_message_iter_open_container(&iter, DBUS_TYPE_VARIANT, "i", &variant));
            assert(dbus_message_iter_append_basic(&variant, DBUS_TYPE_INT32, &id));
            assert(dbus_message_iter_close_container(&iter, &variant));
        }
        if (fields > 3) { assert(dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &timestamp)); }
        if (fields > 4) { assert(dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &id)); }
        expect_error = fields != 4;
        calls = replies = 0;
        assert(MenuHandleEvent(&ctx, NULL, (DBusConnection *)&ctx, msg) == DBUS_HANDLER_RESULT_HANDLED);
        assert(replies == 1 && calls == 0);
        dbus_message_unref(msg);
    }
    expect_error = false;
    for (int mode = 0; mode < 2; ++mode) {
        destroy = mode == 0;
        calls = replies = refs = 0;
        connected = true;
        for (int id = 1; id <= 2; ++id) {
            SDL_DBusMenuItem *item = SDL_calloc(1, sizeof(*item));
            assert(item);
            item->id = id;
            item->_parent.cb = clicked;
            assert(SDL_ListAppend(&registered_menu, item));
        }
        DBusMessage *msg = dbus_message_new_method_call("org.test.Menu", DBUS_MENU_OBJECT_PATH, DBUS_MENU_INTERFACE, "EventGroup");
        assert(msg);
        dbus_message_set_serial(msg, 1);
        DBusMessageIter iter, array;
        dbus_message_iter_init_append(msg, &iter);
        assert(dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "(isvu)", &array));
        for (dbus_int32_t id = 1; id <= 2; ++id) {
            DBusMessageIter event, variant;
            const char *name = "clicked";
            dbus_int32_t value = 0;
            dbus_uint32_t time = 0;
            assert(dbus_message_iter_open_container(&array, DBUS_TYPE_STRUCT, NULL, &event));
            assert(dbus_message_iter_append_basic(&event, DBUS_TYPE_INT32, &id));
            assert(dbus_message_iter_append_basic(&event, DBUS_TYPE_STRING, &name));
            assert(dbus_message_iter_open_container(&event, DBUS_TYPE_VARIANT, "i", &variant));
            assert(dbus_message_iter_append_basic(&variant, DBUS_TYPE_INT32, &value));
            assert(dbus_message_iter_close_container(&event, &variant));
            assert(dbus_message_iter_append_basic(&event, DBUS_TYPE_UINT32, &time));
            assert(dbus_message_iter_close_container(&array, &event));
        }
        assert(dbus_message_iter_close_container(&iter, &array));
        assert(MenuHandleEventGroup(&ctx, registered_menu, (DBusConnection *)&ctx, msg) == DBUS_HANDLER_RESULT_HANDLED);
        assert(calls == (destroy ? 1 : 2));
        assert(refs == 0 && !registered_menu);
        dbus_message_unref(msg);
    }
    SDL_Quit();
    return 0;
}
