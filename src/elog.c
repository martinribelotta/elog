#include <elog.h>

#include <string.h>

elog_t *elog_init(void *arena, size_t size)
{
    elog_t *ptr = (elog_t*) arena;
    ptr->buflen = size;
    ptr->offset = 0;
    return ptr;
}

int elog_put(elog_t *log, const char *const msg, int n, long args[])
{
    elog_entry_t *e = (elog_entry_t*) (log->buffer + log->offset);
    long esize = n * sizeof(long);
    long newoff = log->offset + sizeof(elog_entry_t) + esize;
    if (newoff < log->buflen) {
        e->msgid = (size_t) msg;
        e->len = n;
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
        size_t incr = sizeof(elog_entry_t) + e->len * sizeof(long);
        off += incr;
        func(e, ctx);
    }
    log->offset = 0;
}
