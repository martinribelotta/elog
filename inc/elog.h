#ifndef __ELOG_H__
#define __ELOG_H__

#include <stddef.h>

#include "elog-cpp.h"

#define ELOG(o, msg, ...) do { \
    __attribute__((section("elog"))) static const char p_msg[] = msg;  \
    IF_ELSE(HAS_ARGS(__VA_ARGS__))(    \
        elog_put(o, p_msg, ELOG_NARG(__VA_ARGS__), (long[]){ long_cast_apply(__VA_ARGS__) }); \
    )(                                 \
        elog_put(o, p_msg, 0, (long[]){}); \
    ) \
} while(0)

#define ELOG_BUFFER(name) ((elog_t*)(name))

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    size_t buflen;
    size_t offset;
    char buffer[0];
} elog_t;

typedef struct
{
    size_t msgid;
    size_t len;
    long data[0];
} elog_entry_t;

typedef void (elog_flush_func_t)(elog_entry_t *e, void *ctx);

extern elog_t *elog_init(void *arena, size_t size);
extern int elog_put(elog_t *log, const char *const msg, int n, long args[]);
extern elog_entry_t *elog_peek(elog_t *log);
extern void elog_flush(elog_t *log, elog_flush_func_t func, void *ctx);

#ifdef __cplusplus
}
#endif

#endif /* __ELOG_H__ */
