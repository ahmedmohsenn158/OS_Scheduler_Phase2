#pragma once
#include "processData.h"
#include <stdbool.h>
typedef struct PCB{
    int id;
    int pid;
    int arrival_time;
    int priority;
    int running_time;
    int remaining_time;
    int start_time;
    int memsize;
    int cont_time;
    int stop_time;
    int last_stop_time;  // Tracks the last stop time for waiting calculation
    int finish_time;
    int response_time;
    int waiting_time; // New field for accumulated waiting time
    float turnaround_time;
    float weighted_turnaround_time;
    bool state;
    int start_memory_address;
}PCB;

void initialize_pcb(PCB* process, int start_time){

    process->start_time = start_time;
    process->response_time = start_time - process->arrival_time;
    process->remaining_time = process->running_time;
    process->state = true;
    process->pid = -1;
    process->finish_time = 0;
    process->turnaround_time = 0;
    process->weighted_turnaround_time = 0;
}

void finalize_pcb(PCB* pcb, int finishtime){
    pcb->finish_time=finishtime;
    pcb->response_time=pcb->start_time-pcb->arrival_time;
    pcb->turnaround_time=finishtime - pcb->arrival_time;
    pcb->weighted_turnaround_time=(float)pcb->turnaround_time/(float)pcb->running_time;
}

struct PCB initialize_pcb_RR(processData process,int starttime){
    PCB pcb;
    pcb.arrival_time=process.arrivaltime;
    pcb.running_time=process.runningtime;
    pcb.remaining_time=process.runningtime;
    pcb.id=process.id;
    pcb.priority=process.priority;
    pcb.remaining_time=process.runningtime;
    pcb.running_time=process.runningtime;
    pcb.start_time=starttime;
    pcb.finish_time=0;
    pcb.response_time=0;
    pcb.turnaround_time=0;
    pcb.weighted_turnaround_time=0;

    return pcb;
}