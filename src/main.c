#include <ucmsis.h>
#include <stddef.h>
#include <usemihosting.h>

int main()
{
    static const char const b[] = "Hello world\n";
    int fd = open("tmp.txt", SYS_OPEN_WO);
    if (fd != -1) {
        write(fd, b, sizeof(b));
        close(fd);
    } else {
        writestr("Error opening file");
    }
    return 0;
}
