
.SHELLFLAGS = -Eeuc
# Execute all commands for a recipe in a single shell instance
.ONESHELL:

CC = gcc
DEBUG = -DNDEBUG

CFLAGS = \
  $(DEBUG) \
  -std=c18 \
  -Wno-unused-result -Wsign-compare -fwrapv \
  -Wall -Wextra -Wold-style-definition  \
  -pthread \
  -fPIC

EXES = glibc_alloc_frag realloc_align_check
OBJS = $(EXES:=.o)

.PHONY: all
all: $(EXES)

$(EXES): $(@:=.o)

$(OBJS): $(@:.o=.c)

.PHONY: clean
clean:
	rm -f $(OBJS) $(EXES)
