/*
 * Demonstration of glibc aligned alloc fragmentation bug
 * https://sourceware.org/bugzilla/show_bug.cgi?id=14581
 *
 * Caution: this program may consume a lot of memory!
 */
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * In the below, experimentally:
 * "Fails"/"works" means does/doesn't cause fragmentation leaks (respectively)
 * All three values need to be of the failing class to actually fail
 *
 * Tested with GLIBC_2.35
 */
// 16 works, 32 fails, 64 fails, 128 works, 256 fails
#define ALIGNMENT 32
// 1, 2, 4, 8, 16 all fail, 32, 64, 128 all work (MiB)
#define LARGE_ALLOC_SIZE (16 * 1024 * 1024)
// <= 24 works, > 24 fails
#define SMALL_ALLOC_SIZE 25

void do_trial(void) {
    // Large temporary buffers for "the computation"
    void *tmp1 = aligned_alloc(ALIGNMENT, LARGE_ALLOC_SIZE);
    void *tmp2 = aligned_alloc(ALIGNMENT, LARGE_ALLOC_SIZE);

    // Ensure memory is actually mapped
    memset(tmp1, 0, LARGE_ALLOC_SIZE);
    memset(tmp2, 0, LARGE_ALLOC_SIZE);

    // Leak only a small buffer for "the result"
    malloc(SMALL_ALLOC_SIZE);

    free(tmp1);
    free(tmp2);
}

int main(void) {
    struct timespec one_ms = {0, 1000000 };
    for (int i = 0; i < 500; i++) {
        // Short sleep to make memory consumption easier to observe
        nanosleep(&one_ms, NULL);
        do_trial();
    }
}
