#include "headers.h"
#include <stdatomic.h>

/* Modify this file as needed*/
atomic_int remainingtime;
int cycle_time;
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
        while(getClk() == cycle_time){
            continue;
        }
        atomic_fetch_sub(&remainingtime, 1);
        cycle_time = getClk();
        // remainingtime--;
        // sleep(1);
    }
    printf("Process with pid=%d finished at time %d\n", getpid(), getClk());
    exit(0);
}

void handler(int signum){
    cycle_time = getClk();
}
