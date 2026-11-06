/* Regression: callbacks may destroy or replace a menu during EventGroup. */
#include "../src/core/linux/SDL_dbus.c"
#undef NDEBUG
#include <assert.h>

static SDL_ListNode *registered_menu;
static int replies, calls, refs;
static bool connected, destroy;
static bool expect_error;
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
    ++replies;
    return true;
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
int main(int argc, char **argv)
{
    SDL_DBusContext ctx = { 0 };
#define USE(name) ctx.name = dbus_##name
    USE(message_new_method_return); USE(message_iter_init_append);
    USE(message_new_error);
    USE(message_iter_open_container); USE(message_iter_close_container);
    USE(message_unref); USE(message_iter_init); USE(message_iter_get_arg_type);
    USE(message_iter_recurse); USE(message_iter_get_basic); USE(message_iter_next);
#undef USE
    ctx.connection_ref = retain_connection;
    ctx.connection_unref = release_connection;
    ctx.connection_send = send_reply;
    ctx.connection_get_is_connected = is_connected;
    ctx.connection_get_object_path_data = get_menu;
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
