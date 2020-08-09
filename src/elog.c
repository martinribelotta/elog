#include <elog.h>

#include <limits.h>
#include <string.h>

#define MSGPTR_LEN_BITS (CHAR_BIT / 2)
#define MSGPTR_LEN_BITS_OFFSET ((sizeof(msgptr_t) * CHAR_BIT) - MSGPTR_LEN_BITS)
#define MSGPTR_MSG_MASK ((1 << MSGPTR_LEN_BITS_OFFSET) - 1)
#define MSGPTR_LEN_MASK ((1 << MSGPTR_LEN_BITS) - 1)
#define MSGPTR_LEN(v) (((v) >> MSGPTR_LEN_BITS_OFFSET) & MSGPTR_LEN_MASK)
#define MSGPTR_MSG(v) ((v) & MSGPTR_MSG_MASK)
#define MSGPTR_MAKE(l, p) (((p) & MSGPTR_MSG_MASK) | (((l) & MSGPTR_LEN_MASK) << MSGPTR_LEN_BITS_OFFSET))

elog_t *elog_init(void *arena, size_t size)
{
    elog_t *ptr = (elog_t*) arena;
    ptr->buflen = size;
    ptr->offset = 0;
    return ptr;
}

int elog_put(elog_t *log, const char *const msg, int n, msgparam_t args[])
{
    elog_entry_t *e = (elog_entry_t*) (log->buffer + log->offset);
    long esize = n * sizeof(msgparam_t);
    long newoff = log->offset + sizeof(elog_entry_t) + esize;
    if (newoff < log->buflen) {
        e->msgid = MSGPTR_MAKE(n, (msgptr_t) msg);
        memcpy(e->data, args, esize);
        log->offset = newoff;
        return 1;
    } else {
        return 0;
    }
}

void elog_flush(elog_t *log, elog_flush_func_t func, void *ctx)
{
    long off = 0;
    while (off < log->offset) {
        elog_entry_t *e = ((elog_entry_t*) (log->buffer + off));
        size_t len = MSGPTR_LEN(e->msgid) * sizeof(long);
        size_t incr = sizeof(elog_entry_t) + len;
        off += incr;
        func(e, incr, ctx);
    }
    log->offset = 0;
}
