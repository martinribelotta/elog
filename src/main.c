#include <ucmsis.h>
#include <usemihosting.h>

#include <elog.h>

static elog_t *logger;
static char arena[1024];

static void log_to_semihost(elog_entry_t *e, int len, void *ctx)
{
    int fd = *(int*) ctx;
    write(fd, e, len);
}

int main()
{
    static const char const b[] = "Hello world\n";
    logger = elog_init(arena, sizeof(arena));
    ELOG(logger, "Hello world %d\n", 10);
    const char *const filename = "tmp.txt";
    int fd = open(filename, SYS_OPEN_WO);
    ELOG(logger, "Open file with fd=%d\n", fd);
    if (fd != -1) {
        ELOG(logger, "Writing message to file\n");
        write(fd, b, sizeof(b));
        ELOG(logger, "Closing..\n");
        close(fd);
    } else {
        ELOG(logger, "Error opening archive\n");
        writestr("Error opening file");
    }
    int log_fd = open("bin.log", SYS_OPEN_WOB);
    if (log_fd != -1) {
        elog_flush(logger, log_to_semihost, &log_fd);
        close(log_fd);
    } else {
        writestr("cannot open bin log for write");
    }
    return 0;
}
