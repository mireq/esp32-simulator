#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define ets_printf printf


static void esp_error_check_failed_print(const char *msg, esp_err_t rc, const char *file, int line, const char *function, const char *expression)
{
	ets_printf("%s failed: esp_err_t 0x%x", msg, rc);
#ifdef CONFIG_ESP_ERR_TO_NAME_LOOKUP
	ets_printf(" (%s)", esp_err_to_name(rc));
#endif //CONFIG_ESP_ERR_TO_NAME_LOOKUP
}

void __attribute__((noreturn)) invoke_abort() {
	portEXIT_CRITICAL();
	for (;;) {
	}
}

void __attribute__((noreturn)) _esp_error_check_failed(esp_err_t rc, const char *file, int line, const char *function, const char *expression)
{
	esp_error_check_failed_print("ESP_ERROR_CHECK", rc, file, line, function, expression);
	invoke_abort();
}
