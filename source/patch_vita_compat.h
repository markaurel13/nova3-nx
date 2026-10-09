#include "../runtime/source/error.h"
#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include "../runtime/source/so_util.h"

#include <switch.h>

#define kuKernelCpuUnrestrictedMemcpy(dest, src, size) so_patch_code(dest, src, size)
#define sceClibSnprintf snprintf
#define sceKernelGetProcessTimeWide() (armGetSystemTick() * 10 / 192)

typedef pthread_mutex_t SceKernelLwMutexWork;
typedef pthread_mutex_t SceKernelLwMutexWork;

static inline int sceKernelCreateLwMutex(SceKernelLwMutexWork *workarea, const char *name, int attr, int count, void *opt) {
    pthread_mutexattr_t mattr;
    pthread_mutexattr_init(&mattr);
    pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);
    return pthread_mutex_init(workarea, &mattr);
}

static inline int sceKernelLockLwMutex(SceKernelLwMutexWork *workarea, int count, void *opt) {
    return pthread_mutex_lock(workarea);
}

static inline int sceKernelUnlockLwMutex(SceKernelLwMutexWork *workarea, int count) {
    return pthread_mutex_unlock(workarea);
}

static inline int sceKernelGetThreadId(void) {
    return (int)armGetSystemTick(); // Dummy thread ID, usually just used for logging or recursive mutex tracking which pthread handles natively.
}

 // replaced ("[trace] "); printf(__VA_ARGS__); printf("\n"); } while(0)

#define SCE_KERNEL_MEMBLOCK_TYPE_USER_RX (0x0C20D050)
static inline int kuKernelAllocMemBlock(const char *name, int type, size_t size, void *opt) {
    return 1; // Dummy memory block ID
}
static inline int kuKernelMemProtect(int uid, void **out_base, size_t *out_size) {
    // Return dummy values
    if (out_base) *out_base = malloc(1024*1024);
    if (out_size) *out_size = 1024*1024;
    return 0;
}

// Polyfills for kubridge hooking functions
typedef struct {
    uintptr_t addr;
    uint32_t orig_instr[2];
    uint32_t patch_instr[2];
} so_hook;

extern uintptr_t g_patch_base;
extern uintptr_t g_patch_head;
extern uintptr_t g_patch_size;

static inline so_hook hook_addr(uintptr_t target, uintptr_t destination) {
    so_hook guard;
    guard.addr = target;
    // Copy original instructions
    memcpy(guard.orig_instr, (void*)target, 8);
    // Hook it using android32's hook_arm
    hook_arm(target, destination);
    // Copy the new patched instructions back to guard
    memcpy(guard.patch_instr, (void*)target, 8);
    return guard;
}

#define kuKernelFlushCaches(addr, size) so_flush_caches(&so_mod)
#define runtime_trace(...) do { debugPrintf("[trace] "); debugPrintf(__VA_ARGS__); debugPrintf("\n"); } while(0)
