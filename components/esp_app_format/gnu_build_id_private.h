/* GNU build-ID helpers shared with the standalone host test. */
#pragma once

#include <stddef.h>
#include <stdint.h>

// Startup only: this helper reads flash while the cache is enabled.
static inline void esp_app_format_cache_build_id(const volatile uint8_t *note, size_t note_size,
                                                uint8_t *cached_id, size_t id_size)
{
    if (note == NULL || note_size < 16 || id_size > note_size - 16) {
        return;
    }
    // ELF32 little-endian: namesz=4, descsz=id_size, type=NT_GNU_BUILD_ID, name="GNU".
    if (note[0] != 4 || note[1] || note[2] || note[3] ||
        note[4] != id_size || note[5] || note[6] || note[7] ||
        note[8] != 3 || note[9] || note[10] || note[11] ||
        note[12] != 'G' || note[13] != 'N' || note[14] != 'U' || note[15]) {
        return;
    }
    for (size_t i = 0; i < id_size; ++i) {
        cached_id[i] = note[16 + i];
    }
}

// Always inline into the IRAM API; volatile stores forbid a flash memcpy call.
static inline __attribute__((always_inline)) int esp_app_format_copy_build_id(
    uint8_t *dst, size_t size, const uint8_t *cached_id, size_t id_size)
{
    if (dst == NULL || size == 0) {
        return 0;
    }
    const size_t n = size < id_size ? size : id_size;
    volatile uint8_t *out = dst;
    for (size_t i = 0; i < n; ++i) {
        out[i] = cached_id[i];
    }
    return n;
}
