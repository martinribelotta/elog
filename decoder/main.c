#include <stdio.h>
#include <stdlib.h>

#include <elog-internal.h>

// The buffer size is selected as one page
#define BUF_SIZE 4096
#define STR_CHUNK_SIZE 4096

static char buffer[BUF_SIZE];

#define DIE_HERE() die(__FILE__, __LINE__);

static void die(const char *file, int line)
{
    static char str[128];
    snprintf(str, sizeof(str), "file: %s, line %d", file, line),
    perror(str);
    exit(-1);
}

const char *bmf_get_str(FILE *f, size_t off)
{
    size_t chunk_size = STR_CHUNK_SIZE;
    char *chunk = malloc(chunk_size);
    if (!chunk) {
        DIE_HERE();
    }
    char *ptr = chunk;
    if (fseek(f, off, SEEK_SET) == -1) {
        return NULL;
    }
    while (!feof(f)) {
        int c = fgetc(f);
        if ((c == -1) || (c == 0)) {
            break;
        }
        *ptr++ = c;
        if (ptr - chunk > chunk_size) {
            chunk_size += STR_CHUNK_SIZE;
            chunk = realloc(chunk, chunk_size);
            if (!chunk) {
                DIE_HERE();
            }
        }
    }
    *ptr = 0;
    chunk = realloc(chunk, ptr - chunk + 1);
    if (!chunk) {
        DIE_HERE();
    }
    return chunk;
}

/*
 * Hey! Yes, this is not a real solution but you cannot make
 * va_list dinamically in a portable way in C for now.
 * 
 * The real solution is to write printf-like function with
 * this specific behavioural...
 */
static void long_vprintf(const char *fmt, int argc, const msgparam_t argv[])
{
    switch (argc)
    {
    case 0: printf("%s", fmt); break;
    case 1: printf(fmt, argv[0]); break;
    case 2: printf(fmt, argv[0], argv[1]); break;
    case 3: printf(fmt, argv[0], argv[1], argv[2]); break;
    case 4: printf(fmt, argv[0], argv[1], argv[2], argv[3]); break;
    case 5: printf(fmt, argv[0], argv[1], argv[2], argv[3], argv[4]); break;
    case 6: printf(fmt, argv[0], argv[1], argv[2], argv[3], argv[4], argv[5]); break;
    case 7: printf(fmt, argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]); break;
    case 8: printf(fmt, argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]); break;
    default: // D'oh... only 9 parameters max for now...
    case 9: printf(fmt, argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8]); break;
    }
}

static size_t process_buffer(void *buf, size_t n, FILE *bmf)
{
    size_t process_off = 0;
    while (process_off < n) {
        elog_entry_t *e = (elog_entry_t*) (buf + process_off);
        int len = MSGPTR_LEN(e->msgid);
        int entry_size = sizeof(elog_entry_t) + len * sizeof(msgparam_t);
        if ((process_off + entry_size) >= n) {
            // Insuficient data in buffer, return and expect more data
            break;
        }
        process_off += entry_size;
        msgparam_t off = MSGPTR_MSG(e->msgid);
        const char *msg = bmf_get_str(bmf, off);
        if (msg) {
            long_vprintf(msg, len, e->data);
            free((void*) msg);
        } else {
            printf("Unknown message id 0x%08X len 0x%01X\n", off, len);
            return 0;
        }
    }
    return process_off;
}

int main(int argc, const char *const *argv)
{
    if (argc < 2) {
        printf("usage: %s <binary message file> [input file]\n", argv[0]);
        return -1;
    }
    FILE *bmf = fopen(argv[1], "rb");
    if (!bmf) {
        DIE_HERE();
    }
    size_t readed = 0;
    FILE *in = stdin;
    if (argc > 2) {
        in = fopen(argv[2], "rb");
        if (!in) {
            DIE_HERE();
        }
    }
    while (!feof(in)) {
        readed = fread(buffer + readed, 1, BUF_SIZE - readed, in);
        if (readed == -1) {
            break;
        }
        if (readed > 0) {
            readed -= process_buffer(buffer, readed, bmf);
        }
    }
    fclose(in);
    fclose(bmf);
    return 0;
}
