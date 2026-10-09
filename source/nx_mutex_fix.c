#include <switch.h>
#include <stdbool.h>

#define HANDLE_WAIT_MASK 0x40000000u

#define dmb()          __asm__ __volatile__ ("dmb " : : : "memory")
#define LIKELY(expr)   (__builtin_expect_with_probability(!!(expr), 1, 1.0))
#define UNLIKELY(expr) (__builtin_expect_with_probability(!!(expr), 0, 1.0))

// From libnx internal ThreadVars
typedef struct {
    u32 magic;
    Handle handle;
    union { u64 thread_ptr64; void* thread_ptr; };
    union { u64 reent64; void* reent; };
    union { u64 tls_tp64; void* tls_tp; };
} ThreadVars;

static inline ThreadVars* _getThreadVars(void) {
    return (ThreadVars*)((u8*)armGetTls() + 0x200 - sizeof(ThreadVars));
}

static inline u32 _GetTag(void) {
    ThreadVars *tv = _getThreadVars();
    return tv ? tv->handle : 0;
}

static inline u32 _LoadExclusive(Mutex *ptr) {
    u32 value;
    __asm__ __volatile__("ldrex %r[value], %[ptr]" : [value]"=&r"(value) : [ptr]"Q"(*ptr) : "memory");
    return value;
}

static inline bool _StoreExclusive(Mutex *ptr, u32 value) {
    u32 result;
    __asm__ __volatile__("strex %r[result], %r[value], %[ptr]" : [result]"=&r"(result) : [value]"r"(value), [ptr]"Q"(*ptr) : "memory");
    return result != 0;
}

static inline void _ClearExclusive(void) {
    __asm__ __volatile__("clrex" ::: "memory");
}

void mutexLock(Mutex* m) {
    if (!m) return;
    const u32 cur_handle = _GetTag();
    if (cur_handle == 0) return;

    u32 value = _LoadExclusive(m);
    while (true) {
        if (LIKELY(value == INVALID_HANDLE)) {
            if (UNLIKELY(_StoreExclusive(m, cur_handle) != 0)) {
                value = _LoadExclusive(m);
                continue;
            }
            break;
        }

        if (LIKELY((value & HANDLE_WAIT_MASK) == 0)) {
            if (UNLIKELY(_StoreExclusive(m, value | HANDLE_WAIT_MASK) != 0)) {
                value = _LoadExclusive(m);
                continue;
            }
        }

        // Ask the kernel to arbitrate the lock for us.
        svcArbitrateLock(value & ~HANDLE_WAIT_MASK, (u32*)m, cur_handle);

        value = _LoadExclusive(m);
        if (LIKELY((value & ~HANDLE_WAIT_MASK) == cur_handle)) {
            _ClearExclusive();
            break;
        }
    }

    dmb();
}

bool mutexTryLock(Mutex* m) {
    if (!m) return false;
    const u32 cur_handle = _GetTag();
    if (cur_handle == 0) return false;

    while (true) {
        u32 value = _LoadExclusive(m);
        if (UNLIKELY(value != INVALID_HANDLE)) {
            break;
        }

        dmb();

        if (LIKELY(_StoreExclusive(m, cur_handle) == 0)) {
            return true;
        }
    }

    _ClearExclusive();
    dmb();
    return false;
}

void mutexUnlock(Mutex* m) {
    if (!m) return;
    const u32 cur_handle = _GetTag();
    if (cur_handle == 0) return;

    u32 value = _LoadExclusive(m);
    while (true) {
        if (UNLIKELY(value != cur_handle)) {
            _ClearExclusive();
            break;
        }

        dmb();

        if (LIKELY(_StoreExclusive(m, INVALID_HANDLE) == 0)) {
            break;
        }

        value = _LoadExclusive(m);
    }

    dmb();

    // Only arbitrate unlock if WE held the lock and there were waiters
    if ((value & ~HANDLE_WAIT_MASK) == cur_handle && (value & HANDLE_WAIT_MASK)) {
        svcArbitrateUnlock((u32*)m);
    }
}

bool mutexIsLockedByCurrentThread(const Mutex* m) {
    if (!m) return false;
    const u32 cur_handle = _GetTag();
    if (cur_handle == 0) return false;
    return (*m & ~HANDLE_WAIT_MASK) == cur_handle;
}

void rmutexLock(RMutex* m) {
    if (!m) return;
    if (!mutexIsLockedByCurrentThread(&m->lock)) {
        mutexLock(&m->lock);
    }
    m->counter++;
}

bool rmutexTryLock(RMutex* m) {
    if (!m) return false;
    if (!mutexIsLockedByCurrentThread(&m->lock)) {
        if (!mutexTryLock(&m->lock)) {
            return false;
        }
    }
    m->counter++;
    return true;
}

void rmutexUnlock(RMutex* m) {
    if (!m) return;
    if (--m->counter == 0) {
        mutexUnlock(&m->lock);
    }
}
