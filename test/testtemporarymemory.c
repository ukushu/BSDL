/* Test the event temporary-memory failure paths without changing SDL's global
 * allocator after initialization or adding test hooks to the library. */
// Test-only inclusion of an internal module: use the public SDL symbols.
#include "../src/dynapi/SDL_dynapi.h"
#ifdef SDL_DYNAMIC_API
#undef SDL_DYNAMIC_API
#endif
#define SDL_DYNAMIC_API 0
#include "../src/SDL_internal.h"

#include <SDL3/SDL.h>
#include <stdio.h>

static bool fail_set_tls;
static bool test_SetTLS(SDL_TLSID *id, const void *value, SDL_TLSDestructorCallback destructor)
{
    if (fail_set_tls) {
        fail_set_tls = false;
        return false;
    }
    return SDL_SetTLS(id, value, destructor);
}

#define SDL_SetTLS test_SetTLS
#include "../src/events/SDL_events_temporary_memory.c"
#undef SDL_SetTLS

static SDL_malloc_func original_malloc;
static SDL_calloc_func original_calloc;
static SDL_realloc_func original_realloc;
static SDL_free_func original_free;
static size_t fail_malloc_size;
static size_t fail_calloc_size;
static bool watch_state_allocation;
static void *watched[4];
static bool freed[4];

static void *SDLCALL test_malloc(size_t size)
{
    if (size == fail_malloc_size) {
        fail_malloc_size = 0;
        return NULL;
    }
    return original_malloc(size);
}

static void *SDLCALL test_calloc(size_t count, size_t size)
{
    if (count == 1 && size == fail_calloc_size) {
        fail_calloc_size = 0;
        return NULL;
    }
    void *ptr = original_calloc(count, size);
    if (watch_state_allocation && count == 1 && size == sizeof(SDL_TemporaryMemoryState)) {
        watched[1] = ptr;
    }
    return ptr;
}

static void *SDLCALL test_realloc(void *ptr, size_t size)
{
    return original_realloc(ptr, size);
}

static void SDLCALL test_free(void *ptr)
{
    for (int i = 0; i < SDL_arraysize(watched); ++i) {
        if (ptr == watched[i]) {
            freed[i] = true;
        }
    }
    original_free(ptr);
}

#define CHECK(condition) do { if (!(condition)) { fprintf(stderr, "failed at line %d: %s\n", __LINE__, #condition); return 1; } } while (0)

int main(int argc, char *argv[])
{
    SDL_TemporaryMemory *first, *second, *list;
    SDL_Event event;
    void *payload;

    SDL_GetOriginalMemoryFunctions(&original_malloc, &original_calloc, &original_realloc, &original_free);
    CHECK(SDL_SetMemoryFunctions(test_malloc, test_calloc, test_realloc, test_free));

    payload = SDL_malloc(101);
    CHECK(payload != NULL);
    watched[0] = payload;
    fail_calloc_size = sizeof(SDL_TemporaryMemoryState);
    CHECK(SDL_FreeLater(payload) == NULL);
    CHECK(fail_calloc_size == 0 && freed[0]);

    SDL_zeroa(watched);
    SDL_zeroa(freed);
    payload = SDL_malloc(100);
    CHECK(payload != NULL);
    watched[0] = payload;
    watch_state_allocation = true;
    fail_set_tls = true;
    CHECK(SDL_FreeLater(payload) == NULL);
    watch_state_allocation = false;
    CHECK(!fail_set_tls && watched[1] != NULL && freed[0] && freed[1]);

    SDL_zeroa(watched);
    SDL_zeroa(freed);
    first = SDL_malloc(sizeof(*first));
    second = SDL_malloc(sizeof(*second));
    CHECK(first && second);
    first->memory = SDL_malloc(102);
    second->memory = SDL_malloc(103);
    CHECK(first->memory && second->memory);
    first->next = second;
    second->next = NULL;
    list = first;
    watched[0] = first;
    watched[1] = second;
    watched[2] = first->memory;
    watched[3] = second->memory;

    fail_calloc_size = sizeof(SDL_TemporaryMemoryState);
    CHECK(!SDL_CanTransferTemporaryMemoryFromEvent(list));
    CHECK(fail_calloc_size == 0 && list == first && !freed[0] && !freed[2]);
    fail_calloc_size = sizeof(SDL_TemporaryMemoryState);
    SDL_TransferTemporaryMemoryFromEvent(&list);
    CHECK(fail_calloc_size == 0 && list == NULL);
    CHECK(freed[0] && freed[1] && freed[2] && freed[3]);

    SDL_zeroa(watched);
    SDL_zeroa(freed);
    CHECK(SDL_GetTemporaryMemoryState(true) != NULL);
    payload = SDL_malloc(104);
    CHECK(payload != NULL);
    watched[0] = payload;
    fail_malloc_size = sizeof(SDL_TemporaryMemory);
    CHECK(SDL_FreeLater(payload) == NULL);
    CHECK(fail_malloc_size == 0 && freed[0]);

    SDL_zeroa(watched);
    SDL_zeroa(freed);
    payload = SDL_AllocateTemporaryMemory(105);
    CHECK(payload != NULL);
    watched[0] = payload;
    SDL_FreeTemporaryMemory();
    CHECK(freed[0]);

    SDL_zeroa(watched);
    SDL_zeroa(freed);
    SDL_zero(event);
    event.type = SDL_EVENT_NOTIFICATION_ACTION_INVOKED;
    event.notification.action_id = SDL_CreateTemporaryString("action");
    CHECK(event.notification.action_id != NULL);
    watched[0] = (void *)event.notification.action_id;
    list = NULL;
    SDL_TransferTemporaryMemoryToEvent(&list, &event);
    CHECK(list != NULL);
    SDL_FreeTemporaryMemory();
    CHECK(!freed[0]);
    SDL_TransferTemporaryMemoryFromEvent(&list);
    CHECK(list == NULL);
    SDL_FreeTemporaryMemory();
    CHECK(freed[0]);

    SDL_zero(event);
    event.type = SDL2_SYSWMEVENT;
    SDL2_SysWMmsg message;
    SDL2_SysWMmsg **event_message = (SDL2_SysWMmsg **)((&event.common) + 1);
    *event_message = &message;
    list = NULL;
    fail_malloc_size = sizeof(SDL_TemporaryMemory);
    SDL_TransferTemporaryMemoryToEvent(&list, &event);
    CHECK(fail_malloc_size == 0 && *event_message == NULL && list == NULL);
    return 0;
}
