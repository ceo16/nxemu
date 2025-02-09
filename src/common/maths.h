#pragma once
#include <stdint.h>

uint64_t mull128_u64(uint64_t x, uint64_t y, uint64_t * high);
uint64_t div128_to_64(uint64_t u1, uint64_t u0, uint64_t v, uint64_t * r);
