#ifndef NP_ALIGNED_ALLOC_H
#define NP_ALIGNED_ALLOC_H

#include <stdio.h>

#define log(level, fmt, ...) (fprintf(stderr, "%s: [%s] " fmt "\n", __func__, level, ##__VA_ARGS__))
#ifndef DEBUG
#define debug(fmt, ...) ((void)(1))
#else
#define debug(fmt, ...)   (log("debug  ", fmt, ##__VA_ARGS__))
#endif
#define info(fmt, ...)    (log("info   ", fmt, ##__VA_ARGS__))
#define warning(fmt, ...) (log("warning", fmt, ##__VA_ARGS__))

void *np_malloc(void *ctx, size_t size);
void *np_calloc(void *ctx, size_t nelem, size_t elsize);
void np_free(void *ctx, void *ptr, size_t size);
void *np_realloc(void *ctx, void *ptr, size_t new_size);

#endif
