#include "server.h"
#include "client.h"

int main(int argc, char *argv[])
{
    if (argc >= 2 && strcmp(argv[1], "server") == 0){
        server_mode();
        return 0;
    }
    if (argc >= 2 && strcmp(argv[1], "client") == 0){
        server_mode();
        return 0;
    }
}