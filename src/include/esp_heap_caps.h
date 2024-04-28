#pragma once

#include <stdint.h>
#include <stddef.h>

#define MALLOC_CAP_SPIRAM 0
#define MALLOC_CAP_DEFAULT 0


void *heap_caps_malloc(size_t size, uint32_t caps);
void heap_caps_free(void *ptr);
