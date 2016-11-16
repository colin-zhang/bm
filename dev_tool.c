#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>


int main(int argc, char *argv[])
{
    int opt_index = 0, opt = 0;
    int loop = 0;
    int slotid;

    struct option long_opts[] = {
        {"slot", required_argument, 0, 's'},
        {"help", no_argument, 0, 'h'},
        {"loop", no_argument, 0, 'l'},
        {"board", no_argument, 0, 1},
        {"reboot", optional_argument, 0, 2},
        {"master", no_argument, 0, 3},
        {"self", no_argument, 0, 4},
        {"poll", no_argument, 0, 5},
        {"sendreboot", no_argument, 0, 6},
        {"ssh", required_argument, 0, 7},
        {"rexe", required_argument, 0, 8},
        {"udpcli", required_argument, 0, 9},
        {"udpsrv", no_argument, 0, 10},
    };

    return 0;
}