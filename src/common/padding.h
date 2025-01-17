#pragma once

#define DO_PADDING_CONCAT(x, y) x##y
#define PADDING_CONCAT(x, y) DO_PADDING_CONCAT(x, y)

#define PADDING_BYTES(num_bytes) uint8_t PADDING_CONCAT(pad, __LINE__)[num_bytes]
#define PADDING_WORDS(num_words) uint32_t PADDING_CONCAT(pad, __LINE__)[num_words]
