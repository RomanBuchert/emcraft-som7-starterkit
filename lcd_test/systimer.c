/*
 * systimer.c
 *
 *  Created on: 07.12.2021
 *      Author: RBuchert
 */

#include "systimer.h"
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>

extern void int2led(void);

static uint64_t systick = 0;
static uint64_t** timeouts = NULL ;
static size_t allocated_timeouts = 0;

static uint32_t systick_is_initialized = 0;

/**
 * Local prototypes
 */
static size_t systimer_find_var_index(uint64_t *var);
static size_t systimer_find_emtpy_index();
static size_t systimer_find_free_index(uint64_t* var);
static inline void systimer_handle_timeouts();

/**
 * Global functions
 */
void systimer_init() {
	if (systick_is_initialized) return;
	timeouts = NULL;
	allocated_timeouts = 0;
	systick_set_frequency(1000, rcc_ahb_frequency);
	systick_counter_enable();
	systick_interrupt_enable();
	systick_is_initialized = 1;
}

void systimer_delay_ms(uint64_t ms) {
	uint64_t end = systick + ms;
	while (systick < end) {
		;
	}
}

uint64_t systimer_get_ms() {
	return systick;
}

int systimer_add_timeout(uint64_t* var, uint64_t timeout) {
	if (systimer_find_free_index(var) != SIZE_MAX) {
		*var = timeout;
		errno = 0;
		return 0;
	}
	errno = ENOMEM;
	return -1;
}

int systimer_remove_timeout(uint64_t* var) {
	size_t index = systimer_find_var_index(var);
	if (index != SIZE_MAX) {
		timeouts[index] = NULL;
		errno = 0;
		return 0;
	}
	errno = ENOENT;
	return -1;
}

/**
 * Local functions
 */

static size_t systimer_find_var_index(uint64_t *var) {
	// is var already in list
	for (size_t i = 0; i < allocated_timeouts; i++) {
		if (timeouts[i] == var) {
			return i;
		}
	}
	// if we are here, var is not in list
	return SIZE_MAX;
}

static size_t systimer_find_emtpy_index() {
	// is there an free index in list
	for (size_t i = 0; i < allocated_timeouts; i++) {
		if (timeouts[i] == NULL) {
			return i;
		}
	}
	return SIZE_MAX;
}

static size_t systimer_find_free_index(uint64_t* var) {
	size_t index = SIZE_MAX;
	// At first, lookup if var is already in list.
	index = systimer_find_var_index(var);
	if (index != SIZE_MAX) {
		timeouts[index] = var;
		return index;
	}
	//Var is not in list, find empty list entry
	index = systimer_find_emtpy_index();
	if (index != SIZE_MAX) {
		timeouts[index] = var;
		return index;
	}
 	// resize timeoutlist
	uint64_t** new_timeouts = realloc(timeouts, (allocated_timeouts + 1) * sizeof(uint64_t*));
	if (new_timeouts != NULL) {
		timeouts = new_timeouts;
		timeouts[allocated_timeouts] = var;
		allocated_timeouts += 1;
		return allocated_timeouts - 1;
	}
	// if we are here, no new timers could be allocated.
	return SIZE_MAX;
}

static inline void systimer_handle_timeouts() {
	for(size_t i = 0; i < allocated_timeouts; i++) {
		if ((timeouts[i] != NULL) && (*(timeouts[i]) > 0)) {
			*timeouts[i] -= 1;
		}
	}
}

/**
 * Systick interrupt handler
 */
void sys_tick_handler() {
	systick++;
	systimer_handle_timeouts();
	int2led();
}

