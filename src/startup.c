#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "lwip/dns.h"
#include "lwip/init.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/timeouts.h"
#include "netif/etharp.h"
#include "netif/tapif.h"


static const char *TAG = "startup";


static SemaphoreHandle_t wait_for_tcpip_semaphore;


void app_main();
static void run_main(void *data);
static void exit_app(void *data);


static void tcpip_init_done_signal(void *params) {
	xSemaphoreGive(wait_for_tcpip_semaphore);
}

static void setup_network(void *parameters) {
	static struct netif netif;
	static ip4_addr_t ipaddr, netmask, gw;
	ESP_LOGI(TAG, "initializing tcpip");
	wait_for_tcpip_semaphore = xSemaphoreCreateBinary();
	volatile s32_t tcpipdone = 0;
	tcpip_init(tcpip_init_done_signal, NULL);
	xSemaphoreTake(wait_for_tcpip_semaphore, portMAX_DELAY);
	vSemaphoreDelete(wait_for_tcpip_semaphore);

	IP4_ADDR(&gw, 10, 0, 0, 1);
	IP4_ADDR(&ipaddr, 10, 0, 0, 2);
	IP4_ADDR(&netmask, 255, 255, 255, 0);

	if (!netif_add(&netif, &ipaddr, &netmask, &gw, NULL, tapif_init, ethernet_input)) {
		LWIP_ASSERT("Net interface failed to initialize\r\n", 0);
	}

	ip_addr_t dnsserver;
	IP_ADDR4(&dnsserver, 8, 8, 8, 8);
	dns_setserver(0, &dnsserver);

	netif_set_default(&netif);
	netif_set_up(&netif);

	ESP_LOGI(TAG, "done initializing tcpip");

	xTaskCreate(&run_main, "run_main", 2048, NULL, 5, NULL);

	vTaskDelete(NULL);
}


int main(void) {
	xTaskCreate(setup_network, "setup_network", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1), (xTaskHandle *) NULL);
	vTaskStartScheduler();
	return 0;
}


void vApplicationIdleHook(void) {
	sleep(1);
}


void vMainQueueSendPassed(void) {
}


static void exit_app(void *data) {
	vTaskEndScheduler();
}


static void run_main(void *data) {
	app_main();
	vTaskDelete(NULL);
}
