#ifndef __USEMIHOSTING_H__
#define __USEMIHOSTING_H__

#include <string.h>

typedef enum {
    SYS_CLOSE = 0x02,
    SYS_CLOCK = 0x10,
    SYS_ELAPSED = 0x30,
    SYS_ERRNO = 0x13,
    SYS_FLEN = 0x0C,
    SYS_GET_CMDLINE = 0x15,
    SYS_HEAPINFO = 0x16,
    SYS_ISERROR = 0x08,
    SYS_ISTTY = 0x09,
    SYS_OPEN = 0x01,
    SYS_READ = 0x06,
    SYS_READC = 0x07,
    SYS_REMOVE = 0x0E,
    SYS_RENAME = 0x0F,
    SYS_SEEK = 0x0A,
    SYS_SYSTEM = 0x12,
    SYS_TICKFREQ = 0x31,
    SYS_TIME = 0x11,
    SYS_TMPNAM = 0x0D,
    SYS_WRITE = 0x05,
    SYS_WRITEC = 0x03,
    SYS_WRITE0 = 0x04,
} SemihostFunction_t;

//              mode	0	1	2	3	4	5	6	7	8	9	10	11
// ISO C fopen modea	r	rb	r+	r+b	w	wb	w+	w+b	a	ab	a+	a+b
typedef enum {
    SYS_OPEN_RO = 0,
    SYS_OPEN_ROB = 1,
    SYS_OPEN_RW = 2,
    SYS_OPEN_RWB = 3,
    SYS_OPEN_WO = 4,
    SYS_OPEN_WOB = 5,
    SYS_OPEN_WA = 6,
    SYS_OPEN_WAB = 7,
    SYS_OPEN_AC = 8,
    SYS_OPEN_AB = 9,
    SYS_OPEN_AR = 10,
    SYS_OPEN_ARB = 11,
} SemihostOpenMode_t;

__attribute__((naked))
static unsigned int call_semihosting(unsigned int func, void *params)
{
    (void) func;
    (void) params;
    __asm__ volatile("bkpt #0xAB\nbx lr");
}

static inline void write(int fd, const void *p, size_t n)
{
    void *params[] = {
        (void*) fd,
        (void*) p,
        (void*) n,
    };
    call_semihosting(SYS_WRITE, params);
}

static inline int open(const char *path, int mode)
{
    void *params[] = {
        (void*) path,
        (void*) mode,
        (void*) strlen(path)
    };
    return call_semihosting(SYS_OPEN, params);
}

static inline void close(int fd)
{
    void *params[] = { (void*) fd };
    call_semihosting(SYS_CLOSE, params);
}

static inline void writestr(const char *s)
{
    void *params[] = { (void*) s };
    call_semihosting(SYS_WRITE0, params);
}

#endif /* __USEMIHOSTING_H__ */
