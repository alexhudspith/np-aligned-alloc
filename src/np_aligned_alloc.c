/*
 * Implementations for Numpy aligned memory allocator.
 * Note this file has no direct dependency on Python or Numpy
 * so it can be compiled and tested independently.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef __APPLE__
#include <malloc/malloc.h>
#define malloc_usable_size malloc_size
#else
#include <malloc.h>
#endif

#include "np_aligned_alloc.h"

#define UNUSED(x) x __attribute__((__unused__))

// Overflow-safe add and multiply, return false if overflowed
#if defined __has_builtin && \
    __has_builtin (__builtin_mul_overflow) && \
    __has_builtin (__builtin_add_overflow)
    #define checked_mul_size(a, b, c) (!__builtin_mul_overflow(a, b, c))
    #define checked_add_size(a, b, c) (!__builtin_add_overflow(a, b, c))
#else
    #warning No size_t overflow checking available

    static inline bool checked_mul_size(size_t a, size_t b, size_t *c) {
        *c = a * b;
        return true;
    }

    static inline bool checked_add_size(size_t a, size_t b, size_t *c) {
        *c = a + b;
        return true;
    }
#endif


#define ALIGNMENT ((size_t)64)
#define REALLOC_MIN_BYTES ((size_t)128 * 1024)

static inline bool is_aligned(void *p) {
    return ((uintptr_t)p & ((uintptr_t)ALIGNMENT - 1)) == 0;
}

// Round up to ALIGNMENT, checking for overflow
static inline bool align_up(size_t size, size_t *result) {
    if (!checked_add_size(size, ALIGNMENT - 1, result)) {
        return false;
    }

    *result &= -ALIGNMENT;
    return true;
}

#undef max
#undef min
static inline size_t min(size_t a, size_t b) {
    return a < b ? a : b;
}

static inline void* do_malloc(size_t aligned_size) {
    // Some implementations of aligned_alloc require size to be a multiple of alignment
    return aligned_alloc(ALIGNMENT, aligned_size);
}

void *np_malloc(void *UNUSED(ctx), size_t size) {
    debug("%zu", size);

    size_t aligned_size;
    if (!align_up(size, &aligned_size)) {
        return NULL;
    }
    return do_malloc(aligned_size);
}

void *np_calloc(void *UNUSED(ctx), size_t nelem, size_t elsize) {
    debug("%zu, %zu", nelem, elsize);

    size_t size;
    if (!checked_mul_size(nelem, elsize, &size)) {
        // Overflow
        return NULL;
    }

    size_t aligned_size;
    if (!align_up(size, &aligned_size)) {
        // Overflow
        return NULL;
    }

    void *p = do_malloc(aligned_size);
    if (p != NULL) {
        memset(p, 0, aligned_size);
    }

    return p;
}

void np_free(void *UNUSED(ctx), void *ptr, size_t UNUSED(size)) {
    debug("%p", ptr);
    free(ptr);
}

void *np_realloc(void *UNUSED(ctx), void *ptr, size_t new_size) {
    debug("%p, %zu", ptr, new_size);

    size_t new_aligned_size;
    if (!align_up(new_size, &new_aligned_size)) {
        return NULL;
    }

    if (ptr == NULL) {
        return do_malloc(new_aligned_size);
    }

    assert(new_aligned_size != 0);
    assert(is_aligned(ptr));

    size_t usable_size = malloc_usable_size(ptr);
    size_t old_usable_size = usable_size;
    void *re_ptr = NULL;

    /* Heuristics:
     * 1) Large reallocs are likely to be aligned automatically, e.g. via mmap
     *    (with glibc, only if the original was aligned, which it is)
     * 2) reallocs that are <= the existing backing block size,
     *    will likely remain in place.
     */
    if (new_aligned_size <= usable_size || usable_size >= REALLOC_MIN_BYTES) {
        re_ptr = realloc(ptr, new_aligned_size);
        if (re_ptr == NULL) {
            return NULL;
        }

        if (is_aligned(re_ptr)) {
            return re_ptr;
        }

        warning("Wasted realloc: aligned %p (%zu) -> unaligned %p (%zu)",
            ptr, old_usable_size, re_ptr, new_aligned_size);
        // Old ptr is now invalid, replaced by re_ptr
        ptr = re_ptr;
        // Update usable size for correct memcpy later
        usable_size = min(usable_size, malloc_usable_size(re_ptr));
    }

    // Malloc and copy
    void *new_ptr = do_malloc(new_aligned_size);
    if (new_ptr == NULL) {
        // If re_ptr is NULL we didn't realloc, so the given ptr is still valid.
        // Return NULL to signal error.
        if (re_ptr == NULL) {
            return NULL;
        }

        // Trouble: the original allocation is no longer valid either
        warning("Failed to allocate aligned block after unaligned realloc. Aborting.");
        abort();
    }

    debug("copying old_usable_size=%zu, usable_size=%zu, new_size=%zu, old_ptr=%p, ptr=%p, new_ptr=%p",
        old_usable_size, usable_size, new_aligned_size, old_ptr, ptr, new_ptr);

    // Copy only the amount that is known to exist in the source block
    memcpy(new_ptr, ptr, min(new_aligned_size, usable_size));
    free(ptr);

    return new_ptr;
}
