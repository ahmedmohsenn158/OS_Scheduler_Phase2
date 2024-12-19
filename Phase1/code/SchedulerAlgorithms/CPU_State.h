#pragma once
typedef struct CPU_State
{
    float cpu_utilization;
    float avg_wta;
    float avg_waiting;
    int totalRunningTime;
    float totalWTA ;
    int totalWaiting ;
}CPU_State;

CPU_State create_cpu_state(){
    CPU_State cpu;
    cpu.totalRunningTime=0;
    cpu.totalWaiting=0;
    cpu.totalWTA=0;
    return cpu;
}

void update_cpu_state(CPU_State *cpu,float WTA,int waiting){
    cpu->totalWaiting+=waiting;
    cpu->totalWTA+=WTA;
}

void finalize_cpu_state(CPU_State *cpu,int num_of_processes,int idle_time,int total_time){
    cpu->totalRunningTime=total_time-idle_time;
    cpu->cpu_utilization=(float)cpu->totalRunningTime/total_time*100;
    cpu->avg_waiting=(float)cpu->totalWaiting/num_of_processes;
    cpu->avg_wta=(float)cpu->totalWTA/num_of_processes;
}