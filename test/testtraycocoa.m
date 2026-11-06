/* Exercise Cocoa tray image conversion and geometry. */
#include "../src/tray/cocoa/SDL_tray.m"
#undef NDEBUG
#include <assert.h>

static void SDLCALL tray_entry_callback(void *userdata, SDL_TrayEntry *entry)
{
    (void)entry;
    ++*(int *)userdata;
}

int main(int argc, char **argv)
{
    @autoreleasepool {
        SDL_TrayEntry entry = { 0 };
        entry.nsitem = [[NSMenuItem alloc] initWithTitle:@"Command" action:nil keyEquivalent:@""];
        SDL_SetTrayEntryEnabled(&entry, false);
        assert(!SDL_GetTrayEntryEnabled(&entry));
        SDL_SetTrayEntryEnabled(&entry, true);
        assert(SDL_GetTrayEntryEnabled(&entry));

        SDL_Surface *source = SDL_CreateSurface(1028, 820, SDL_PIXELFORMAT_RGBA32);
        assert(source);
        assert(SDL_FillSurfaceRect(source, NULL, SDL_MapSurfaceRGBA(source, 24, 48, 72, 128)));
        NSImage *image = CreateTrayImage(source, NSMakeSize(22, 18), true);
        assert(image && image.isTemplate);
        assert(NSEqualSizes(image.size, NSMakeSize(22, 18)));
        NSBitmapImageRep *rep = (NSBitmapImageRep *)image.representations.firstObject;
        assert(rep.pixelsWide == 1028 && rep.pixelsHigh == 820);
        assert(rep.bitmapData[0] == 24 && rep.bitmapData[3] == 128);
        SDL_DestroySurface(source);
        // The native representation must own its pixels after conversion.
        assert(rep.bitmapData[0] == 24 && rep.bitmapData[3] == 128);

        source = SDL_CreateSurface(44, 22, SDL_PIXELFORMAT_RGBA32);
        assert(source);
        image = CreateTrayImage(source, NSZeroSize, false);
        assert(image && !image.isTemplate);
        assert(NSEqualSizes(image.size, NSMakeSize(22, 22)));
        SDL_DestroySurface(source);

        SDL_PropertiesID props = SDL_CreateProperties();
        assert(props);
        SDL_SetMainReady();
        assert(SDL_Init(0));
        assert(SDL_SetNumberProperty(props, SDL_PROP_TRAY_CREATE_ICON_WIDTH_NUMBER, 22));
        assert(!SDL_CreateTrayWithProperties(props)); // Height is missing.
        assert(SDL_SetNumberProperty(props, SDL_PROP_TRAY_CREATE_ICON_HEIGHT_NUMBER, -1));
        assert(!SDL_CreateTrayWithProperties(props));
        SDL_DestroyProperties(props);

        assert(SDL_InitSubSystem(SDL_INIT_VIDEO));
        SDL_Surface *tray_icon = SDL_CreateSurface(16, 16, SDL_PIXELFORMAT_RGBA32);
        assert(tray_icon);
        SDL_Tray *tray = SDL_CreateTray(tray_icon, "Tray geometry test");
        assert(tray);

        int callback_count = 0;
        SDL_TrayMenu *menu = SDL_CreateTrayMenu(tray);
        assert(menu);
        assert(!SDL_CreateTrayMenu(tray));
        SDL_TrayEntry *checkbox = SDL_InsertTrayEntryAt(menu, -1, "Check", SDL_TRAYENTRY_CHECKBOX);
        assert(checkbox);
        assert(!SDL_CreateTraySubmenu(checkbox));
        SDL_SetTrayEntryCallback(checkbox, tray_entry_callback, &callback_count);
        assert([NSApp sendAction:checkbox->nsitem.action to:checkbox->nsitem.target from:checkbox->nsitem]);
        assert(callback_count == 1 && SDL_GetTrayEntryChecked(checkbox));

        SDL_TrayEntry *submenu_entry = SDL_InsertTrayEntryAt(menu, -1, "More", SDL_TRAYENTRY_SUBMENU);
        assert(submenu_entry);
        SDL_TrayMenu *submenu = SDL_CreateTraySubmenu(submenu_entry);
        assert(submenu);
        assert(!SDL_CreateTraySubmenu(submenu_entry));
        SDL_TrayEntry *submenu_item = SDL_InsertTrayEntryAt(submenu, -1, "Run", SDL_TRAYENTRY_BUTTON);
        assert(submenu_item);
        SDL_SetTrayEntryCallback(submenu_item, tray_entry_callback, &callback_count);
        assert([NSApp sendAction:submenu_item->nsitem.action to:submenu_item->nsitem.target from:submenu_item->nsitem]);
        assert(callback_count == 2);

        SDL_Rect tray_rect = { 0, 0, 0, 0 };
        assert(SDL_GetTrayRect(tray, &tray_rect));
        assert(tray_rect.w > 0 && tray_rect.h > 0);
        SDL_DestroyTray(tray);
        SDL_DestroySurface(tray_icon);
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        SDL_Quit();
    }
    return 0;
}
