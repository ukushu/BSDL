/* Regression: callbacks may destroy or replace a menu during EventGroup. */
#include "../src/core/linux/SDL_dbus.c"
#include "../src/tray/unix/SDL_dbustray.c"
#undef NDEBUG
#include <assert.h>

static SDL_ListNode *registered_menu;
static int replies, calls, refs;
static bool connected, destroy;
static bool expect_error;
static bool expect_getall_reply;
static bool expect_window_id_reply;

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
static void retain_connection(DBusConnection *conn) { ++refs; }
static void release_connection(DBusConnection *conn) { --refs; }
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
    assert(dbus_message_get_type(msg) == (expect_error ? DBUS_MESSAGE_TYPE_ERROR : DBUS_MESSAGE_TYPE_METHOD_RETURN));
    if (expect_error) {
        assert(SDL_strcmp(dbus_message_get_error_name(msg), DBUS_ERROR_INVALID_ARGS) == 0);
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
    int index = 0;
    assert(dbus_message_iter_init(msg, &array));
    dbus_message_iter_recurse(&array, &entry);
    while (dbus_message_iter_get_arg_type(&entry) != DBUS_TYPE_INVALID) {
        dbus_message_iter_recurse(&entry, &pair);
        dbus_message_iter_get_basic(&pair, &name);
        if (SDL_strcmp(name, "shortcut") == 0) {
            assert(dbus_message_iter_next(&pair));
            dbus_message_iter_recurse(&pair, &value);
            dbus_message_iter_recurse(&value, &sequence);
            dbus_message_iter_recurse(&sequence, &keypress);
            while (dbus_message_iter_get_arg_type(&keypress) == DBUS_TYPE_STRING) {
                dbus_message_iter_get_basic(&keypress, &name);
                assert(index < SDL_arraysize(expected));
                assert(SDL_strcmp(name, expected[index++]) == 0);
                dbus_message_iter_next(&keypress);
            }
            break;
        }
        dbus_message_iter_next(&entry);
    }
    assert(index == SDL_arraysize(expected));
    dbus_message_unref(msg);
}

int main(int argc, char **argv)
{
    SDL_DBusContext ctx = { 0 };
#define USE(name) ctx.name = dbus_##name
    USE(message_new_method_return); USE(message_iter_init_append);
    USE(message_new_error);
    USE(message_iter_open_container); USE(message_iter_close_container);
    USE(message_unref); USE(message_iter_init); USE(message_iter_get_arg_type);
    USE(message_iter_recurse); USE(message_iter_get_basic); USE(message_iter_next);
    USE(message_iter_append_basic);
#undef USE
    ctx.connection_ref = retain_connection;
    ctx.connection_unref = release_connection;
    ctx.connection_send = send_reply;
    ctx.connection_get_is_connected = is_connected;
    ctx.connection_get_object_path_data = get_menu;
    test_tray_icon_byte_order();
    test_tray_entry_without_callback();
    test_tray_getall_without_icon_or_tooltip(&ctx);
    test_tray_get_window_id(&ctx);
    test_item_is_menu_matches_left_click_behavior();
    test_shortcut_property(&ctx, SDL_SCANCODE_5, "5");
    test_shortcut_property(&ctx, SDL_SCANCODE_KP_1, "KP_1");
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
    return 0;
}
