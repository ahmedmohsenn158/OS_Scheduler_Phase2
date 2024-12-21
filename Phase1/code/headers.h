#pragma once
#include <stdio.h> //if you don't use scanf/printf change this include
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "processData.h"
#include "pcb.h"
#include "Datastructures/Queue.h"
#include "Datastructures/PriQueue.h"
#include "Datastructures/CircularQueue.h"
#include "Datastructures/Tree.h"

#define SHKEY 300

///==============================
//don't mess with this variable//
int *shmaddr; //
//===============================

struct msgbuff
{
    long mtype;
    PCB process;
};

int getClk()
{
    return *shmaddr;
}

/*
 * All processes call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
*/
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *)shmat(shmid, (void *)0, 0);
}

/*
 * All processes call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
*/

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}

int receive_message_mlf(int msgqueue_id, PCB *process){
    struct msgbuff message;
    int receive_value=msgrcv(msgqueue_id,&message, sizeof(message.process),0,!IPC_NOWAIT);
    if (receive_value==-1){
        perror("Error in receiving the message\n");
        return -1;
    } else {
        printf("Process %d with the following arrival time and priority received successfully: %d %d \n",message.process.id,message.process.arrival_time,message.process.priority);
        *process=message.process;
        return 1;
    }
}

int receive_message_mlf_nowait(int msgqueue_id, PCB* process){
    struct msgbuff message;
    int receive_value = msgrcv(msgqueue_id,&message, sizeof(message.process),0,IPC_NOWAIT);
    if (receive_value == -1){
        return -1;
    } else {
        printf("Process %d with the following arrival time and priority received successfully: %d %d \n",message.process.id,message.process.arrival_time,message.process.priority);
        *process=message.process;
        return 1;
        //processData process=message.process;
        //Queue_Enqueue(readyqueue,process);
        //printf("%d %d %d %d\n",readyqueue->Tail->Data.id,readyqueue->Tail->Data.arrivaltime,readyqueue->Tail->Data.runningtime,readyqueue->Tail->Data.priority);
    }
}

void write_schedulerprf(float utilization,float avg_wta,float avg_waiting){
    FILE *fptr;

    fptr = fopen("scheduler.perf.txt", "w");

    fprintf(fptr, "CPU utilization=%.2f%%\nAvg WTA=%.2f\nAvg Waiting=%.2f\n", utilization, avg_wta, avg_waiting);

    fclose(fptr);
}

void write_schedulerlog_process_started(FILE*fptr,int time,int id,int arrival_time,int running_time, int remaining_time, int waiting_time){
    fprintf(fptr, "At time %d process %d started arr %d total %d remain %d wait %d\n",time,id,arrival_time,running_time,remaining_time,waiting_time);
}

void write_schedulerlog_process_resumed(FILE*fptr,int time,int id,int arrival_time,int running_time, int remaining_time, int waiting_time){
    fprintf(fptr, "At time %d process %d resumed arr %d total %d remain %d wait %d\n",time,id,arrival_time,running_time,remaining_time,waiting_time);
}

void write_schedulerlog_process_Stoped(FILE*fptr,int time,int id,int arrival_time,int running_time, int remaining_time, int waiting_time){
    fprintf(fptr, "At time %d process %d stopped arr %d total %d remain %d wait %d\n",time,id,arrival_time,running_time,remaining_time,waiting_time);
}

void write_schedulerlog_process_finished(FILE* fptr,int time,int id,int arrival_time,int running_time, int remaining_time, int waiting_time, float TA, float WTA){
    fprintf(fptr, "At time %d process %d finished arr %d total %d remain %d wait %d TA %.2f WTA %.2f\n",time,id,arrival_time,running_time,remaining_time,waiting_time,TA,WTA);
}

void write_schedulerlog_process_stopped(FILE*fptr,int time,int id,int arrival_time,int running_time, int remaining_time, int waiting_time){
    fprintf(fptr, "At time %d process %d stopped arr %d total %d remain %d wait %d\n",time,id,arrival_time,running_time,remaining_time,waiting_time);
}

