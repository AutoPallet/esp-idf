/*
 * SPDX-FileCopyrightText: 2026 AutoPallet
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Standalone host test: cc -std=c11 -Wall -Wextra -Werror test_gnu_build_id.c -o test_gnu_build_id */
#include <assert.h>
#include <string.h>
#include "../gnu_build_id_private.h"

int main(void)
{
    uint8_t note[36] = {4, 0, 0, 0, 20, 0, 0, 0, 3, 0, 0, 0, 'G', 'N', 'U', 0};
    uint8_t cached[20] = {0};
    const uint8_t zeros[20] = {0};
    uint8_t dst[24];
    for (size_t i = 0; i < sizeof(cached); ++i) {
        note[16 + i] = i + 1;
    }

    // Before startup (or without a linked note), getter returns zeros.
    assert(esp_app_format_copy_build_id(dst, sizeof(dst), cached, sizeof(cached)) == 20);
    assert(memcmp(dst, zeros, sizeof(cached)) == 0);
    esp_app_format_cache_build_id(NULL, sizeof(note), cached, sizeof(cached));
    assert(memcmp(cached, zeros, sizeof(cached)) == 0);
    for (size_t length = 0; length < sizeof(note); ++length) {
        esp_app_format_cache_build_id(note, length, cached, sizeof(cached));
        assert(memcmp(cached, zeros, sizeof(cached)) == 0);
    }

    // Every header/name byte is validated, including nonzero high bytes.
    for (size_t i = 0; i < 16; ++i) {
        note[i] ^= 0x80;
        esp_app_format_cache_build_id(note, sizeof(note), cached, sizeof(cached));
        assert(memcmp(cached, zeros, sizeof(cached)) == 0);
        note[i] ^= 0x80;
    }
    esp_app_format_cache_build_id(note, sizeof(note), cached, sizeof(cached));
    assert(memcmp(cached, note + 16, sizeof(cached)) == 0);

    // Null, zero, short and oversized destinations: no write past returned size.
    assert(esp_app_format_copy_build_id(NULL, 20, cached, sizeof(cached)) == 0);
    for (size_t length = 0; length <= sizeof(dst); ++length) {
        memset(dst, 0xa5, sizeof(dst));
        const size_t copied = length < sizeof(cached) ? length : sizeof(cached);
        assert(esp_app_format_copy_build_id(dst, length, cached, sizeof(cached)) == (int)copied);
        assert(memcmp(dst, cached, copied) == 0);
        for (size_t i = copied; i < sizeof(dst); ++i) {
            assert(dst[i] == 0xa5);
        }
    }
    return 0;
}
