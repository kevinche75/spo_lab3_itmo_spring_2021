#include <stdio.h>
#include <unistd.h> /* for sleep */
#include <sys/ioctl.h>

#define update() printf("\033[H\033[J")
#define gotoxy(x, y) printf("\033[%d;%dH", x, y)

struct winsize size;

int main(void)
{

    update();
    puts("Hello");
    puts("Line 2");
    sleep(2);
    update();
    gotoxy(0, 0);
    puts("Line 1");
    sleep(2);
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    printf("Columns %d, rows %d", size.ws_col, size.ws_row);
    return(0);
}