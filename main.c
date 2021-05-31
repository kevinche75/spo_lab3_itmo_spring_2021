//#include <stdio.h>
//#include <unistd.h> /* for sleep */
//#include <sys/ioctl.h>
//
//#define update() printf("\033[H\033[J")
//#define gotoxy(x, y) printf("\033[%d;%dH", x, y)
//
//struct winsize size;
//
//int main(void)
//{
//
//    update();
//    puts("Hello");
//    puts("Line 2");
//    sleep(2);
//    update();
//    gotoxy(0, 0);
//    puts("Line 1");
//    sleep(2);
//    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
//    printf("Columns %d, rows %d", size.ws_col, size.ws_row);
//    return(0);
//}

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>

static struct termios stored_settings;

void set_keypress(void)
{
    struct termios new_settings;

    tcgetattr(0,&stored_settings);

    new_settings = stored_settings;

    new_settings.c_lflag &= (~ICANON & ~ECHO);
    new_settings.c_cc[VTIME] = 0;
    new_settings.c_cc[VMIN] = 1;

    tcsetattr(0,TCSANOW,&new_settings);
    return;
}

void reset_keypress(void)
{
    tcsetattr(0,TCSANOW,&stored_settings);
    return;
}

int main(void)
{
    fd_set rfds;
    struct timeval tv;
    int retval;

    set_keypress();

    while(1)
    {
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);

        tv.tv_sec = 0;
        tv.tv_usec = 0;
        retval = select(2, &rfds, NULL, NULL, &tv);
        if (retval)
        {
            printf("Data is available now.\n");
            getc(stdin);
        }
        else
        {
            printf("No data available.\n");
        }
        usleep(100000);
    }
    reset_keypress();
    exit(0);
}