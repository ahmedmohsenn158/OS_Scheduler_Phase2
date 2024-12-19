#pragma once
#include"CPU_State.h"

#include"../processData.h"

void Execute_SJF(int num_of_processes, int msgqueue_id, PriQueue* readyqueue) {
    int received_processes = 0; 
    processData tempMessage;
    PCB message;
    int free_time=0;      
    CPU_State cpu=create_cpu_state();
    FILE* fptr;
    fptr = fopen("scheduler.log.txt", "w");
    while (received_processes < num_of_processes || readyqueue->Count > 0) {
        if (num_of_processes!=received_processes){
            int rec_val = receive_message_mlf_nowait(msgqueue_id, &message);
            while (rec_val == 1 && received_processes<num_of_processes) {
                received_processes++;
                Pri_Enqueue(readyqueue, message, message.running_time);
                rec_val = receive_message_mlf_nowait(msgqueue_id, &message); 
            }
        }
        printf("At time: %d\n",getClk());
        displayPriQueue(readyqueue);

        if (readyqueue->Count > 0) {
            PCB pcb;
            Pri_Dequeue(readyqueue, &pcb, &pcb.running_time);
            printf("Executing process %d with runtime %d\n", pcb.id, pcb.running_time);


            initialize_pcb(&pcb,getClk());
            write_schedulerlog_process_started(fptr,getClk(),pcb.id,pcb.arrival_time,pcb.running_time,pcb.remaining_time,pcb.response_time);
            int process_pid = fork();
            if (process_pid < 0) {
                perror("Error in forking the process");
                continue;
            }

            if (process_pid == 0) {
                char running_time[10];
                snprintf(running_time, sizeof(running_time), "%d", pcb.running_time);
                execl("process.out", "process.out", running_time, NULL);
                perror("Error in execl");
                exit(EXIT_FAILURE);
            } else {
                int stat_loc;
                waitpid(process_pid, &stat_loc, 0);
                finalize_pcb(&pcb,getClk());
                update_cpu_state(&cpu,pcb.weighted_turnaround_time,pcb.response_time);
                write_schedulerlog_process_finished(fptr,getClk(),pcb.id,pcb.arrival_time,pcb.running_time,stat_loc,pcb.response_time,pcb.turnaround_time,pcb.weighted_turnaround_time);
            }
        } else {
            free_time++;
            sleep(1);
        }
    }

    printf("All processes completed, producing the output file \n");
    fclose(fptr);
    int finish_time=getClk();
    free_time++;
    finalize_cpu_state(&cpu,num_of_processes,free_time,finish_time);
    write_schedulerprf(cpu.cpu_utilization,cpu.avg_wta,cpu.avg_waiting);
}