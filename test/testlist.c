/* Regression test for SDL_ListInsertAtPosition inserting at the head. */
#include "../src/SDL_internal.h"

#include <stdio.h>
#include <stdlib.h>

#undef SDL_malloc
#define SDL_malloc malloc
#undef SDL_free
#define SDL_free free

#include "../src/SDL_list.c"

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "failed at line %d: %s\n", __LINE__, #condition); \
        SDL_ListClear(&head); \
        return 1; \
    } \
} while (0)

int main(int argc, char *argv[])
{
    SDL_ListNode *head = NULL;
    int first = 1;
    int second = 2;

    (void)argc;
    (void)argv;

    CHECK(SDL_ListInsertAtPosition(&head, 0, &first));
    CHECK(SDL_ListCountEntries(&head) == 1);
    CHECK(head->entry == &first);

    CHECK(SDL_ListInsertAtPosition(&head, 0, &second));
    CHECK(SDL_ListCountEntries(&head) == 2);
    CHECK(head->entry == &second);
    CHECK(head->next->entry == &first);

    SDL_ListClear(&head);
    return 0;
}
