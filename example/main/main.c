#include "freertos/FreeRTOS.h"

#include "lwip/dns.h"
#include "lwip/err.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"


static const char *TAG = "simulator";


// Declare events
typedef enum {
	NETWORK_DISCONNECTED,
	NETWORK_CONNECTED,
} network_event_t;

ESP_EVENT_DECLARE_BASE(NETWORK_EVENT);
ESP_EVENT_DEFINE_BASE(NETWORK_EVENT);

esp_event_loop_handle_t event_loop;


static void example_network_access() {
	const struct addrinfo hints = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM,
	};

	ESP_LOGI(TAG, "DNS lookup. Address=github.com");

	struct addrinfo *res;
	int err = getaddrinfo("github.com", "443", &hints, &res);

	if (err) {
		ESP_LOGE(TAG, "Cannot resolve: %d", err);
		if (res) {
			freeaddrinfo(res);
		}
		return;
	}

	char address[100];
	inet_ntop(res->ai_family, &((struct sockaddr_in *)res->ai_addr)->sin_addr, address, sizeof(address));
	ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", address);
}


static void on_network_event(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
	switch (event_id) {
		case NETWORK_DISCONNECTED:
			ESP_LOGI(TAG, "Disconnected");
			break;
		case NETWORK_CONNECTED:
			ESP_LOGI(TAG, "Connected");
			example_network_access();
			break;
	}
}


#ifdef SIMULATOR
static void generate_fake_events_task(void *args) {
	// Simulate events
	vTaskDelay(100 / portTICK_PERIOD_MS);
	esp_event_post_to(event_loop, NETWORK_EVENT, NETWORK_CONNECTED, NULL, 0, portMAX_DELAY);
	vTaskDelay(5000 / portTICK_PERIOD_MS);
	esp_event_post_to(event_loop, NETWORK_EVENT, NETWORK_DISCONNECTED, NULL, 0, portMAX_DELAY);
	// Destroy unused task
	vTaskDelete(NULL);
}
#endif


void app_main(void)
{
	ESP_ERROR_CHECK(esp_event_loop_create_default());
#ifdef SIMULATOR
	// Task for sending fake events
	xTaskCreate(generate_fake_events_task, "fake_init", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
#endif

	// New event loop
	esp_event_loop_args_t loop_args = {
		.queue_size = 1,
		.task_name = "event_loop",
		.task_priority = 1,
		.task_stack_size = configMINIMAL_STACK_SIZE,
		.task_core_id = 0,
	};
	ESP_ERROR_CHECK(esp_event_loop_create(&loop_args, &event_loop));
	ESP_ERROR_CHECK(esp_event_handler_register_with(event_loop, NETWORK_EVENT, ESP_EVENT_ANY_ID, on_network_event, NULL));

	for (;;) {
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		ESP_LOGI(TAG, "Hello");
	}
}
