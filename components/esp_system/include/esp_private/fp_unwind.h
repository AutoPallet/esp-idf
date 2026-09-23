/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FP_UNWIND_H
#define FP_UNWIND_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Collect return addresses and saved frame pointers from a frame chain.
 *
 * Stops at an unaligned frame, a frame record outside data RAM, a ROM caller, or
 * the requested depth. At least one output array must be non-NULL. Each supplied
 * array must have room for depth entries. Unwritten entries are left unchanged.
 *
 * @param frame Initial frame pointer, immediately after the two-word frame record
 * @param callers Return addresses, or NULL
 * @param stacks Saved frame pointers, or NULL
 * @param depth Maximum number of entries to collect
 * @return Number of entries written to each supplied array
 */
uint32_t esp_fp_get_callers(uint32_t frame, void **callers, void **stacks, uint32_t depth);

/**
 * @brief Print backtrace for the given execution frame thanks to the frame pointers.
 *
 * @param frame_or Snapshot of the CPU registers when the program stopped its
 *                 normal execution. This frame is usually generated on the
 *                 stack when an exception or an interrupt occurs.
 */
void esp_fp_print_backtrace(const void *frame_or);

#ifdef __cplusplus
}
#endif

#endif // FP_UNWIND_H
