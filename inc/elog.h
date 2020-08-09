#ifndef __ELOG_H__
#define __ELOG_H__

#include <stddef.h>
#include <stdint.h>

#include "elog-cpp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t msgptr_t;
typedef uint32_t msgparam_t;

#define _msgparam_cast_apply(x) ((long) (x)),
#define msgparam_cast_apply(...) EVAL(MAP(_msgparam_cast_apply, __VA_ARGS__))

#define ELOG(o, msg, ...) do { \
    __attribute__((section("elog"))) static const char p_msg[] = msg;  \
    IF_ELSE(HAS_ARGS(__VA_ARGS__))(    \
        elog_put(o, p_msg, ELOG_NARG(__VA_ARGS__), (msgparam_t[]){ msgparam_cast_apply(__VA_ARGS__) }); \
    )(                                 \
        elog_put(o, p_msg, 0, (msgparam_t[]){}); \
    ) \
} while(0)

typedef struct {
    size_t buflen;
    size_t offset;
    char buffer[0];
} elog_t;

typedef struct {
    msgptr_t msgid;
    msgparam_t data[0];
} elog_entry_t;

typedef void (elog_flush_func_t)(elog_entry_t *e, int len, void *ctx);

extern elog_t *elog_init(void *arena, size_t size);
extern int elog_put(elog_t *log, const char *const msg, int n, msgparam_t args[]);
extern elog_entry_t *elog_peek(elog_t *log);
extern void elog_flush(elog_t *log, elog_flush_func_t func, void *ctx);

#ifdef __cplusplus
}
#endif

#endif /* __ELOG_H__ */
