#pragma once

#include <stdbool.h>

#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "sys/queue.h"

#include "esp_event_base.h"

#ifndef SLIST_FOREACH_SAFE
#define	SLIST_FOREACH_SAFE(var, head, field, tvar)			\
	for ((var) = SLIST_FIRST((head));				\
	    (var) && ((tvar) = SLIST_NEXT((var), field), 1);		\
	    (var) = (tvar))
#endif



typedef SLIST_HEAD(base_nodes, base_node) base_nodes_t;

/// Event handler
typedef struct esp_event_handler_instance {
	esp_event_handler_t handler;                                    /**< event handler function*/
	void* arg;                                                      /**< event handler argument */
#ifdef CONFIG_ESP_EVENT_LOOP_PROFILING
	uint32_t invoked;                                               /**< number of times this handler has been invoked */
	int64_t time;                                                   /**< total runtime of this handler across all calls */
#endif
	SLIST_ENTRY(esp_event_handler_instance) next;                   /**< next event handler in the list */
} esp_event_handler_instance_t;

typedef SLIST_HEAD(esp_event_handler_instances, esp_event_handler_instance) esp_event_handler_instances_t;

/// Event
typedef struct esp_event_id_node {
	int32_t id;                                                     /**< id number of the event */
	esp_event_handler_instances_t handlers;                         /**< list of handlers to be executed when
																			this event is raised */
	SLIST_ENTRY(esp_event_id_node) next;                            /**< pointer to the next event node on the linked list */
} esp_event_id_node_t;

typedef SLIST_HEAD(esp_event_id_nodes, esp_event_id_node) esp_event_id_nodes_t;

typedef struct esp_event_base_node {
	esp_event_base_t base;                                          /**< base identifier of the event */
	esp_event_handler_instances_t handlers;                         /**< event base level handlers, handlers for
																			all events with this base */
	esp_event_id_nodes_t id_nodes;                                  /**< list of event ids with this base */
	SLIST_ENTRY(esp_event_base_node) next;                          /**< pointer to the next base node on the linked list */
} esp_event_base_node_t;

typedef SLIST_HEAD(esp_event_base_nodes, esp_event_base_node) esp_event_base_nodes_t;

typedef struct esp_event_loop_node {
	esp_event_handler_instances_t handlers;                         /** event loop level handlers */
	esp_event_base_nodes_t base_nodes;                              /** list of event bases registered to the loop */
	SLIST_ENTRY(esp_event_loop_node) next;                          /** pointer to the next loop node containing
																			event loop level handlers and the rest of
																			event bases registered to the loop */
} esp_event_loop_node_t;

typedef SLIST_HEAD(esp_event_loop_nodes, esp_event_loop_node) esp_event_loop_nodes_t;

/// Event loop
typedef struct esp_event_loop_instance {
	const char* name;                                               /**< name of this event loop */
	QueueHandle_t queue;                                            /**< event queue */
	TaskHandle_t task;                                              /**< task that consumes the event queue */
	TaskHandle_t running_task;                                      /**< for loops with no dedicated task, the
																			task that consumes the queue */
	SemaphoreHandle_t mutex;                                        /**< mutex for updating the events linked list */
	esp_event_loop_nodes_t loop_nodes;                              /**< set of linked lists containing the
																			registered handlers for the loop */
#ifdef CONFIG_ESP_EVENT_LOOP_PROFILING
	atomic_uint_least32_t events_recieved;                          /**< number of events successfully posted to the loop */
	atomic_uint_least32_t events_dropped;                           /**< number of events dropped due to queue being full */
	SemaphoreHandle_t profiling_mutex;                              /**< mutex used for profiliing */
	SLIST_ENTRY(esp_event_loop_instance) next;                      /**< next event loop in the list */
#endif
} esp_event_loop_instance_t;

#if CONFIG_ESP_EVENT_POST_FROM_ISR
typedef union esp_event_post_data {
	uint32_t val;
	void *ptr;
} esp_event_post_data_t;
#else
typedef void* esp_event_post_data_t;
#endif

/// Event posted to the event queue
typedef struct esp_event_post_instance {
#if CONFIG_ESP_EVENT_POST_FROM_ISR
	bool data_allocated;                                             /**< indicates whether data is allocated from heap */
	bool data_set;                                                   /**< indicates if data is null */
#endif
	esp_event_base_t base;                                           /**< the event base */
	int32_t id;                                                      /**< the event id */
	esp_event_post_data_t data;                                      /**< data associated with the event */
} esp_event_post_instance_t;



/// Configuration for creating event loops
typedef struct {
	int32_t queue_size;                         /**< size of the event loop queue */
	const char* task_name;                      /**< name of the event loop task; if NULL,
														a dedicated task is not created for event loop*/
	UBaseType_t task_priority;                  /**< priority of the event loop task, ignored if task name is NULL */
	uint32_t task_stack_size;                   /**< stack size of the event loop task, ignored if task name is NULL */
	BaseType_t task_core_id;                    /**< core to which the event loop task is pinned to,
														ignored if task name is NULL */
} esp_event_loop_args_t;

/**
 * @brief Create a new event loop.
 *
 * @param[in] event_loop_args configuration structure for the event loop to create
 * @param[out] event_loop handle to the created event loop
 *
 * @return
 *  - ESP_OK: Success
 *  - ESP_ERR_NO_MEM: Cannot allocate memory for event loops list
 *  - ESP_FAIL: Failed to create task loop
 *  - Others: Fail
 */
esp_err_t esp_event_loop_create(const esp_event_loop_args_t* event_loop_args, esp_event_loop_handle_t* event_loop);

/**
 * @brief Delete an existing event loop.
 *
 * @param[in] event_loop event loop to delete
 *
 * @return
 *  - ESP_OK: Success
 *  - Others: Fail
 */
esp_err_t esp_event_loop_delete(esp_event_loop_handle_t event_loop);

/**
 * @brief Create default event loop
 *
 * @return
 *  - ESP_OK: Success
 *  - ESP_ERR_NO_MEM: Cannot allocate memory for event loops list
 *  - ESP_FAIL: Failed to create task loop
 *  - Others: Fail
 */
esp_err_t esp_event_loop_create_default(void);

/**
 * @brief Delete the default event loop
 *
 * @return
 *  - ESP_OK: Success
 *  - Others: Fail
 */
esp_err_t esp_event_loop_delete_default(void);

/**
 * @brief Dispatch events posted to an event loop.
 *
 * This function is used to dispatch events posted to a loop with no dedicated task, i.e task name was set to NULL
 * in event_loop_args argument during loop creation. This function includes an argument to limit the amount of time
 * it runs, returning control to the caller when that time expires (or some time afterwards). There is no guarantee
 * that a call to this function will exit at exactly the time of expiry. There is also no guarantee that events have
 * been dispatched during the call, as the function might have spent all of the alloted time waiting on the event queue.
 * Once an event has been unqueued, however, it is guaranteed to be dispatched. This guarantee contributes to not being
 * able to exit exactly at time of expiry as (1) blocking on internal mutexes is necessary for dispatching the unqueued
 * event, and (2) during  dispatch of the unqueued event there is no way to control the time occupied by handler code
 * execution. The guaranteed time of exit is therefore the alloted time + amount of time required to dispatch
 * the last unqueued event.
 *
 * In cases where waiting on the queue times out, ESP_OK is returned and not ESP_ERR_TIMEOUT, since it is
 * normal behavior.
 *
 * @param[in] event_loop event loop to dispatch posted events from
 * @param[in] ticks_to_run number of ticks to run the loop
 *
 * @note encountering an unknown event that has been posted to the loop will only generate a warning, not an error.
 *
 * @return
 *  - ESP_OK: Success
 *  - Others: Fail
 */
esp_err_t esp_event_loop_run(esp_event_loop_handle_t event_loop, TickType_t ticks_to_run);

/**
 * @brief Register an event handler to the system event loop.
 *
 * This function can be used to register a handler for either: (1) specific events,
 * (2) all events of a certain event base, or (3) all events known by the system event loop.
 *
 *  - specific events: specify exact event_base and event_id
 *  - all events of a certain base: specify exact event_base and use ESP_EVENT_ANY_ID as the event_id
 *  - all events known by the loop: use ESP_EVENT_ANY_BASE for event_base and ESP_EVENT_ANY_ID as the event_id
 *
 * Registering multiple handlers to events is possible. Registering a single handler to multiple events is
 * also possible. However, registering the same handler to the same event multiple times would cause the
 * previous registrations to be overwritten.
 *
 * @param[in] event_base the base id of the event to register the handler for
 * @param[in] event_id the id of the event to register the handler for
 * @param[in] event_handler the handler function which gets called when the event is dispatched
 * @param[in] event_handler_arg data, aside from event data, that is passed to the handler when it is called
 *
 * @note the event loop library does not maintain a copy of event_handler_arg, therefore the user should
 * ensure that event_handler_arg still points to a valid location by the time the handler gets called
 *
 * @return
 *  - ESP_OK: Success
 *  - ESP_ERR_NO_MEM: Cannot allocate memory for the handler
 *  - ESP_ERR_INVALID_ARG: Invalid combination of event base and event id
 *  - Others: Fail
 */
esp_err_t esp_event_handler_register(esp_event_base_t event_base,
										int32_t event_id,
										esp_event_handler_t event_handler,
										void* event_handler_arg);

/**
 * @brief Register an event handler to a specific loop.
 *
 * This function behaves in the same manner as esp_event_handler_register, except the additional
 * specification of the event loop to register the handler to.
 *
 * @param[in] event_loop the event loop to register this handler function to
 * @param[in] event_base the base id of the event to register the handler for
 * @param[in] event_id the id of the event to register the handler for
 * @param[in] event_handler the handler function which gets called when the event is dispatched
 * @param[in] event_handler_arg data, aside from event data, that is passed to the handler when it is called
 *
 * @note the event loop library does not maintain a copy of event_handler_arg, therefore the user should
 * ensure that event_handler_arg still points to a valid location by the time the handler gets called
 *
 * @return
 *  - ESP_OK: Success
 *  - ESP_ERR_NO_MEM: Cannot allocate memory for the handler
 *  - ESP_ERR_INVALID_ARG: Invalid combination of event base and event id
 *  - Others: Fail
 */
esp_err_t esp_event_handler_register_with(esp_event_loop_handle_t event_loop,
											esp_event_base_t event_base,
											int32_t event_id,
											esp_event_handler_t event_handler,
											void* event_handler_arg);

/**
 * @brief Unregister a handler with the system event loop.
 *
 * This function can be used to unregister a handler so that it no longer gets called during dispatch.
 * Handlers can be unregistered for either: (1) specific events, (2) all events of a certain event base,
 * or (3) all events known by the system event loop
 *
 *  - specific events: specify exact event_base and event_id
 *  - all events of a certain base: specify exact event_base and use ESP_EVENT_ANY_ID as the event_id
 *  - all events known by the loop: use ESP_EVENT_ANY_BASE for event_base and ESP_EVENT_ANY_ID as the event_id
 *
 * This function ignores unregistration of handlers that has not been previously registered.
 *
 * @param[in] event_base the base of the event with which to unregister the handler
 * @param[in] event_id the id of the event with which to unregister the handler
 * @param[in] event_handler the handler to unregister
 *
 * @return ESP_OK success
 * @return ESP_ERR_INVALID_ARG invalid combination of event base and event id
 * @return others fail
 */
esp_err_t esp_event_handler_unregister(esp_event_base_t event_base, int32_t event_id, esp_event_handler_t event_handler);

/**
 * @brief Unregister a handler with the system event loop.
 *
 * This function behaves in the same manner as esp_event_handler_unregister, except the additional specification of
 * the event loop to unregister the handler with.
 *
 * @param[in] event_loop the event loop with which to unregister this handler function
 * @param[in] event_base the base of the event with which to unregister the handler
 * @param[in] event_id the id of the event with which to unregister the handler
 * @param[in] event_handler the handler to unregister
 *
 * @return
 *  - ESP_OK: Success
 *  - ESP_ERR_INVALID_ARG: Invalid combination of event base and event id
 *  - Others: Fail
 */
esp_err_t esp_event_handler_unregister_with(esp_event_loop_handle_t event_loop,
											esp_event_base_t event_base,
											int32_t event_id,
											esp_event_handler_t event_handler);

/**
 * @brief Posts an event to the system default event loop. The event loop library keeps a copy of event_data and manages
 * the copy's lifetime automatically (allocation + deletion); this ensures that the data the
 * handler recieves is always valid.
 *
 * @param[in] event_base the event base that identifies the event
 * @param[in] event_id the event id that identifies the event
 * @param[in] event_data the data, specific to the event occurence, that gets passed to the handler
 * @param[in] event_data_size the size of the event data
 * @param[in] ticks_to_wait number of ticks to block on a full event queue
 *
 * @return
 *  - ESP_OK: Success
 *  - ESP_ERR_TIMEOUT: Time to wait for event queue to unblock expired, 
 *                      queue full when posting from ISR
 *  - ESP_ERR_INVALID_ARG: Invalid combination of event base and event id
 *  - Others: Fail
 */
esp_err_t esp_event_post(esp_event_base_t event_base,
							int32_t event_id,
							void* event_data,
							size_t event_data_size,
							TickType_t ticks_to_wait);

/**
 * @brief Posts an event to the specified event loop. The event loop library keeps a copy of event_data and manages
 * the copy's lifetime automatically (allocation + deletion); this ensures that the data the
 * handler recieves is always valid.
 *
 * This function behaves in the same manner as esp_event_post_to, except the additional specification of the event loop
 * to post the event to.
 *
 * @param[in] event_loop the event loop to post to
 * @param[in] event_base the event base that identifies the event
 * @param[in] event_id the event id that identifies the event
 * @param[in] event_data the data, specific to the event occurence, that gets passed to the handler
 * @param[in] event_data_size the size of the event data
 * @param[in] ticks_to_wait number of ticks to block on a full event queue
 *
 * @return
 *  - ESP_OK: Success
 *  - ESP_ERR_TIMEOUT: Time to wait for event queue to unblock expired, 
 *                      queue full when posting from ISR
 *  - ESP_ERR_INVALID_ARG: Invalid combination of event base and event id
 *  - Others: Fail
 */
esp_err_t esp_event_post_to(esp_event_loop_handle_t event_loop,
							esp_event_base_t event_base,
							int32_t event_id,
							void* event_data,
							size_t event_data_size,
							TickType_t ticks_to_wait);

#if CONFIG_ESP_EVENT_POST_FROM_ISR
/**
 * @brief Special variant of esp_event_post for posting events from interrupt handlers.
 * 
 * @param[in] event_base the event base that identifies the event
 * @param[in] event_id the event id that identifies the event
 * @param[in] event_data the data, specific to the event occurence, that gets passed to the handler
 * @param[in] event_data_size the size of the event data; max is 4 bytes
 * @param[out] task_unblocked an optional parameter (can be NULL) which indicates that an event task with 
 *                            higher priority than currently running task has been unblocked by the posted event;
 *                            a context switch should be requested before the interrupt is existed.
 *
 * @note this function is only available when CONFIG_ESP_EVENT_POST_FROM_ISR is enabled
 * @note when this function is called from an interrupt handler placed in IRAM, this function should
 *       be placed in IRAM as well by enabling CONFIG_ESP_EVENT_POST_FROM_IRAM_ISR
 *
 * @return
 *  - ESP_OK: Success
 *  - ESP_FAIL: Event queue for the default event loop full
 *  - ESP_ERR_INVALID_ARG: Invalid combination of event base and event id,
 *                          data size of more than 4 bytes 
 *  - Others: Fail
 */
esp_err_t esp_event_isr_post(esp_event_base_t event_base,
							int32_t event_id,
							void* event_data,
							size_t event_data_size,
							BaseType_t* task_unblocked);

/**
 * @brief Special variant of esp_event_post_to for posting events from interrupt handlers
 *
 * @param[in] event_base the event base that identifies the event
 * @param[in] event_id the event id that identifies the event
 * @param[in] event_data the data, specific to the event occurence, that gets passed to the handler
 * @param[in] event_data_size the size of the event data
 * @param[out] task_unblocked an optional parameter (can be NULL) which indicates that an event task with 
 *                            higher priority than currently running task has been unblocked by the posted event;
 *                            a context switch should be requested before the interrupt is existed.
 *
 * @note this function is only available when CONFIG_ESP_EVENT_POST_FROM_ISR is enabled
 * @note when this function is called from an interrupt handler placed in IRAM, this function should
 *       be placed in IRAM as well by enabling CONFIG_ESP_EVENT_POST_FROM_IRAM_ISR
 *
 * @return
 *  - ESP_OK: Success
 *  - ESP_FAIL: Event queue for the loop full
 *  - ESP_ERR_INVALID_ARG: Invalid combination of event base and event id,
 *                          data size of more than 4 bytes 
 *  - Others: Fail
 */
esp_err_t esp_event_isr_post_to(esp_event_loop_handle_t event_loop,
							esp_event_base_t event_base,
							int32_t event_id,
							void* event_data,
							size_t event_data_size,
							BaseType_t* task_unblocked);
#endif

/**
 * @brief Dumps statistics of all event loops.
 *
 * Dumps event loop info in the format:
 *
 @verbatim
	   event loop
		   handler
		   handler
		   ...
	   event loop
		   handler
		   handler
		   ...

  where:

   event loop
	   format: address,name rx:total_recieved dr:total_dropped
	   where:
		   address - memory address of the event loop
		   name - name of the event loop, 'none' if no dedicated task
		   total_recieved - number of successfully posted events
		   total_dropped - number of events unsucessfully posted due to queue being full

   handler
	   format: address ev:base,id inv:total_invoked run:total_runtime
	   where:
		   address - address of the handler function
		   base,id - the event specified by event base and id this handler executes
		   total_invoked - number of times this handler has been invoked
		   total_runtime - total amount of time used for invoking this handler

 @endverbatim
 *
 * @param[in] file the file stream to output to
 *
 * @note this function is a noop when CONFIG_ESP_EVENT_LOOP_PROFILING is disabled
 *
 * @return
 *  - ESP_OK: Success
 *  - ESP_ERR_NO_MEM: Cannot allocate memory for event loops list
 *  - Others: Fail
 */
esp_err_t esp_event_dump(FILE* file);
