#include "headers.h"
#include <stdatomic.h>

/* Modify this file as needed*/
int remainingtime;
int cycle_time;
int time_diff;
void handler(int signum);

int main(int agrc, char *argv[])
{
    initClk();
    signal(SIGCONT, handler);
    int start_time=getClk();
    cycle_time = getClk();
    remainingtime=atoi(argv[1]);
    printf("A process with pid=%d and ppid=%d was forked at time: %d\n",getpid(),getppid(),start_time);
    //TODO The process needs to get the remaining time from somewhere
    //remainingtime = ??;

    while (remainingtime > 0)
    {
        while (time_diff == 0)
            time_diff = cycle_time - getClk();

        remainingtime--;
        cycle_time = getClk();
        time_diff = 0;

        // remainingtime--;
        // sleep(1);
    }
    printf("Process with pid=%d finished at time %d\n", getpid(), getClk());
    exit(0);
}

void handler(int signum){
    cycle_time = getClk();
}
