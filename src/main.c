#include <ucmsis.h>
#include <usemihosting.h>

#include <elog.h>

static elog_t *logger;
static char arena[1024];

int main()
{
    static const char const b[] = "Hello world\n";
    logger = elog_init(arena, sizeof(arena));
    ELOG(logger, "Hello world %d\n", 10);
    const char *const filename = "tmp.txt";
    int fd = open(filename, SYS_OPEN_WO);
    ELOG(logger, "Open file %s with fd=%d\n", filename, fd);
    if (fd != -1) {
        ELOG(logger, "Writing message to file\n");
        write(fd, b, sizeof(b));
        ELOG(logger, "Closing..\n");
        close(fd);
    } else {
        ELOG(logger, "Error opening archive\n");
        writestr("Error opening file");
    }
    return 0;
}
