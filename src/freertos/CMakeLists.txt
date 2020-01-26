set(FREERTOS_SOURCES
	"croutine.c"
	"list.c"
	"queue.c"
	"tasks.c"
	"timers.c"
	"portable/GCC/Posix/port.c"
	"portable/MemMang/heap_3.c"
)
include_directories(
	${ESP_SIMULATOR_INCLUDE_DIRECTORIES}
	"${CMAKE_CURRENT_SOURCE_DIR}/src/FreeRTOS_Posix/FreeRTOS_Kernel/portable/GCC/Posix/"
)
list(TRANSFORM FREERTOS_SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/src/FreeRTOS_Posix/FreeRTOS_Kernel/)
add_library(freertos STATIC ${FREERTOS_SOURCES})
