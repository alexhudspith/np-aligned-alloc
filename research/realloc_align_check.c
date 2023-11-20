/**
 * Experiment to observe alignment after realloc.
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#ifdef __APPLE__
#include <malloc/malloc.h>
#define malloc_usable_size malloc_size
#else
#include <malloc.h>
#include <string.h>

#endif

#define ALIGNMENT 64
//#define do_alloc(a, s) malloc(s)
#define do_alloc(a, s) aligned_alloc(a, s)

#undef min
#undef max
static size_t max(size_t a, size_t b) {
    return a < b ? b : a;
}

static bool write_row(void *p, size_t sz, void *old_p) {
    size_t align_offset;
    size_t slack;

    if (p != NULL) {
        align_offset = (uintptr_t)p % ALIGNMENT;
        slack = malloc_usable_size(p) - sz;
    } else {
        align_offset = 0;
        slack = 0;
    }

    bool unaligned = align_offset != 0;
    if (old_p != NULL) {
        // realloc
        bool moved = (uintptr_t) p != (uintptr_t) old_p;
        char *description = "";
        if (moved && unaligned) {
            description = " realloc moved, unaligned";
        } else if (moved) {
            description = " realloc moved";
        } else if (unaligned) {
            description = " unaligned";
        }

        printf("      y %11zu  %14p     %3lu   %4ld%s\n", sz, p, align_offset, slack, description);
    } else {
        printf("        %11zu  %14p     %3lu   %4ld\n", sz, p, align_offset, slack);
    }

    return unaligned;
}

int main(void) {
    size_t max_size_unaligned = 0;
    bool unaligned;

    printf("                                     Align        \n");
    printf("Realloc?       Size  Pointer         offset  Slack\n");

    for (size_t base_sz = 1; base_sz < 1ul << 33; base_sz <<= 1) {
        // Allocate powers of two + [-1, ..., +2]
        for (size_t sz = base_sz - 1; sz < base_sz + 2; sz++) {
            void *p = do_alloc(ALIGNMENT, sz);
            unaligned = write_row(p, sz, NULL);
            if (unaligned) {
                max_size_unaligned = max(max_size_unaligned, sz);
            }

            // Realloc 1.5 times original size
            size_t sz2 = sz * 3 / 2;
            void *p2 = realloc(p, sz2);
            unaligned = write_row(p2, sz2, p);
            if (unaligned) {
                max_size_unaligned = max(max_size_unaligned, sz2);
            }
        }
    }

    printf("\nMax size unaligned: %zu\n", max_size_unaligned);
}
