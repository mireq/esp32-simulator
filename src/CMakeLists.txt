cmake_minimum_required(VERSION 3.5)

project(simulator)

set(ESP_SIMULATOR_INCLUDE_DIRECTORIES
	"${CMAKE_CURRENT_SOURCE_DIR}/include/"
	"${CMAKE_CURRENT_SOURCE_DIR}/freertos/src/FreeRTOS_Posix"
	"${CMAKE_CURRENT_SOURCE_DIR}/freertos/src/FreeRTOS_Posix/FreeRTOS_Kernel/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/freertos/src/FreeRTOS_Posix/FreeRTOS_Kernel/portable/GCC/Posix"
	"${CMAKE_CURRENT_SOURCE_DIR}/lwip/src/src/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/lwip/contrib/ports/freertos/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/lwip/contrib/ports/unix/port/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/mbedtls/src/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/mbedtls/src/crypto/include"
	"$ENV{IDF_PATH}/components/log/include/"
	"$ENV{IDF_PATH}/components/esp_common/include/"
	"$ENV{IDF_PATH}/components/esp_event/include/"
	"$ENV{IDF_PATH}/components/"
	"${CMAKE_BINARY_DIR}/../../../config/"
)
set(ESP_SIMULATOR_INCLUDE_DIRECTORIES ${ESP_SIMULATOR_INCLUDE_DIRECTORIES} PARENT_SCOPE)

add_subdirectory("freertos")
add_subdirectory("lwip")
add_subdirectory("mbedtls")

include_directories(
	${ESP_SIMULATOR_INCLUDE_DIRECTORIES}
)

add_library(simulator STATIC
	esp_err.c
	esp_event.c
	esp_heap_caps.c
	esp_log.c
	startup.c
	"$ENV{IDF_PATH}/components/esp_common/src/esp_err_to_name.c"
	"$ENV{IDF_PATH}/components/esp_event/esp_event.c"
)

add_definitions(-D_GNU_SOURCE -DSIMULATOR -g3 -ggdb)

target_link_libraries(
	${PROJECT_NAME}
	freertos
	lwip
	mbedtls
	pthread
	rt
)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)
