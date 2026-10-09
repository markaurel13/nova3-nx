#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "so_util.h"

#define kuKernelCpuUnrestrictedMemcpy(dst, src, sz) memcpy(dst, src, sz)
#define fatal_error(...) do { printf("[patch] " __VA_ARGS__); printf("\n"); exit(1); } while(0)
#define runtime_trace(...)

extern so_module so_mod;

