#pragma once

#define FORCE_INLINE __attribute__((always_inline)) inline
#define CAT_(a, b) a ## b
#define CAT(a, b) CAT_(a, b)
#define LINE_NUM(a) CAT(a, __LINE__)
