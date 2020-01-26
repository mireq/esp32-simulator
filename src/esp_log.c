#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>

#include "esp_log.h"


uint32_t esp_log_timestamp(void) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint32_t)tv.tv_sec*1000 + (uint32_t)tv.tv_usec/1000;
}

void esp_log_write(esp_log_level_t level, const char* tag, const char* format, ...) {
	va_list arglist;
	va_start(arglist, format);
	vprintf(format, arglist);
	va_end(arglist);
}
