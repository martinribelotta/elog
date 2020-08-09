#ifndef __ELOG_INTERNAL_H__
#define __ELOG_INTERNAL_H__

#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#define MSGPTR_LEN_BITS (CHAR_BIT / 2)
#define MSGPTR_LEN_BITS_OFFSET ((sizeof(msgptr_t) * CHAR_BIT) - MSGPTR_LEN_BITS)
#define MSGPTR_MSG_MASK ((1 << MSGPTR_LEN_BITS_OFFSET) - 1)
#define MSGPTR_LEN_MASK ((1 << MSGPTR_LEN_BITS) - 1)
#define MSGPTR_LEN(v) (((v) >> MSGPTR_LEN_BITS_OFFSET) & MSGPTR_LEN_MASK)
#define MSGPTR_MSG(v) ((v) & MSGPTR_MSG_MASK)
#define MSGPTR_MAKE(l, p) (((p) & MSGPTR_MSG_MASK) | (((l) & MSGPTR_LEN_MASK) << MSGPTR_LEN_BITS_OFFSET))

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t msgptr_t;
typedef uint32_t msgparam_t;

typedef struct {
    msgptr_t msgid;
    msgparam_t data[0];
} __attribute__((__packed__)) elog_entry_t;

#ifdef __cplusplus
}
#endif

#endif /* __ELOG_INTERNAL_H__ */
