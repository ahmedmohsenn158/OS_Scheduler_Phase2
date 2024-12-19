#pragma once
#include "CPU_State.h"
#include "../processData.h"
#include "../headers.h"
#include "../pcb.h"
#include <stdbool.h>
#include "../Datastructures/PriQueue.h"


void start_process(PCB* running_process) {
    running_process->pid = fork();
}

void HPF_Execute(PriQueue* ProcessQueue, int msgqueue_id, int process_count)
{
    CPU_State cpu=create_cpu_state();
    FILE* fptr;
    fptr = fopen("scheduler.log.txt", "w");

    int timestep = getClk();
    int free_time = 0;
    int first_start;

    //initialise Process Table
    PCB process_table[process_count];
    PCB message;

    int completed_processes = 0;
    int received_processes = 0;

    PCB* running_process = NULL;
    PCB  process;

    while(ProcessQueue->Count > 0 || completed_processes < process_count|| received_processes < process_count)
    {   

        // Get processes in current timestep
        if (process_count != received_processes){
            int rec_val = receive_message_mlf(msgqueue_id, &message);
            while (rec_val == 1 && received_processes < process_count) {
                received_processes++;
                initialize_pcb(&message, getClk());
                process_table[message.id - 1] = message;
                Pri_Enqueue(ProcessQueue, process_table[message.id - 1], process_table[message.id - 1].priority);
                rec_val = receive_message_mlf_nowait(msgqueue_id, &message); 
            }
        }

        if (getClk() != timestep) {
            printf("At time: %d\n",getClk());
            displayPriQueue(ProcessQueue); 
            timestep = getClk();  
        }

        bool NextProcessWithHigherPri = ProcessQueue->Head && running_process && ProcessQueue->Head->Priority < running_process->priority;
        bool NoRunningProcessButProcessArrived = ProcessQueue->Head && running_process == NULL;

        if (NoRunningProcessButProcessArrived) {
            Pri_Dequeue(ProcessQueue, &process, &process.priority);
            running_process = &process;
            if(running_process->id == 1){
                free_time = running_process->arrival_time;
            }
            printf("Executing process %d with runtime %d\n",
                running_process->id, 
                running_process->running_time);

            if (running_process->pid == -1)  {
                initialize_pcb(running_process, getClk());
                write_schedulerlog_process_started(fptr, getClk(), 
                    running_process->id, 
                    running_process->arrival_time, 
                    running_process->running_time, 
                    running_process->remaining_time, 
                    running_process->response_time);
                running_process->cont_time = getClk();
                start_process(running_process);
            } else {
                running_process->cont_time = getClk();
                kill(running_process->pid, SIGCONT);
                write_schedulerlog_process_resumed(fptr, running_process->cont_time, 
                    running_process->id, 
                    running_process->arrival_time, 
                    running_process->running_time, 
                    running_process->remaining_time, 
                    (running_process->cont_time - running_process->stop_time));
            }
        }
        
        
        // Check if running_process finished
        if (running_process) {
            int status;
            if(waitpid(running_process->pid, &status, WNOHANG) == running_process->pid){
                // TODO: update cpu state
                int time = getClk();
                finalize_pcb(running_process, getClk());
                update_cpu_state(&cpu, running_process->weighted_turnaround_time, (running_process->turnaround_time - running_process->running_time));
                write_schedulerlog_process_finished(fptr, time, 
                    running_process->id, 
                    running_process->arrival_time, 
                    running_process->running_time, 
                    status, 
                    (running_process->turnaround_time - running_process->running_time), 
                    running_process->turnaround_time, 
                    running_process->weighted_turnaround_time);
                running_process = NULL;
                completed_processes++;
                continue;
                
            }
        }

        // Handle process with higher priority.
        if (NextProcessWithHigherPri) {
            // We have a process with a higher priority to run.
            running_process->stop_time = getClk();
            kill(running_process->pid, SIGSTOP);
            running_process->remaining_time -= (getClk() - running_process->cont_time);
            write_schedulerlog_process_stopped(fptr, getClk(), 
                running_process->id, 
                running_process->arrival_time, 
                running_process->running_time, 
                running_process->remaining_time, 
                running_process->response_time);
            Pri_Enqueue(ProcessQueue, process, running_process->priority);
            Pri_Dequeue(ProcessQueue, &process, &process.priority);
            running_process = &process;

            printf("Executing process %d with runtime %d\n",
                    running_process->id, 
                    running_process->running_time);

            if (running_process->pid == -1) {
                int time = getClk();
                initialize_pcb(running_process, time);
                write_schedulerlog_process_started(fptr, time, 
                    running_process->id, 
                    running_process->arrival_time, 
                    running_process->running_time, 
                    running_process->remaining_time, 
                    running_process->response_time);
                running_process->cont_time = getClk();
                start_process(running_process);
            } else {
                running_process->cont_time = getClk();
                kill(running_process->pid, SIGCONT);
                write_schedulerlog_process_resumed(fptr, running_process->cont_time, 
                    running_process->id, 
                    running_process->arrival_time, 
                    running_process->running_time, 
                    running_process->remaining_time, 
                    ((running_process->cont_time - running_process->stop_time)));
            }          
        } 

        if(running_process->pid == 0){
            char running_time[10];
            snprintf(running_time, sizeof(running_time), "%d", running_process->running_time);
            execl("process.out", "process.out", running_time, NULL);
            perror("Error in execl");
            exit(EXIT_FAILURE);
        }

        if (!running_process && ProcessQueue->Count == 0) {
           free_time++;
           while(getClk() == timestep){
               continue;
           }
           timestep = getClk();
        }
    }

    printf("All processes completed, producing the output file \n");
    fclose(fptr);
    printf("The free time: %d \n", free_time);
    finalize_cpu_state(&cpu,process_count, free_time, getClk());
    write_schedulerprf(cpu.cpu_utilization, cpu.avg_wta, cpu.avg_waiting);
}
