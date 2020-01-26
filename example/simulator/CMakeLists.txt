cmake_minimum_required(VERSION 3.5)

project(example)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../src ${CMAKE_CURRENT_BINARY_DIR}/simulator)

include_directories(
	${ESP_SIMULATOR_INCLUDE_DIRECTORIES}
)

add_executable(
	${PROJECT_NAME}
	../main/main.c
)

target_link_libraries(
	${PROJECT_NAME}
	simulator
)

add_definitions(-D_GNU_SOURCE -DSIMULATOR -g3 -ggdb)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_ROOT_BUILD_DIR}")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "example")
