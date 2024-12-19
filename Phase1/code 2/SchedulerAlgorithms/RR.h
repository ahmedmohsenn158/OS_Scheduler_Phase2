#pragma once
#include "CPU_State.h"
#include "../processData.h"
#include "../pcb.h"
#include "../Datastructures/CircularQueue.h"


void Execute_RR(int num_of_processes, int msgqueue_id, CircularQueue *readyQueue, int quantum) {
    int received_processes = 0;
    int completed_processes = 0;
    int free_time = 0;

    CPU_State cpu = create_cpu_state();
    FILE *fptr = fopen("scheduler.log.txt", "w");

    while (completed_processes < num_of_processes) {
        while (received_processes < num_of_processes) {
            PCB newPCB;
            int rec_val = receive_message_mlf_nowait(msgqueue_id, &newPCB);

            if (rec_val == 1) {
                received_processes++;
                initialize_pcb(&newPCB, getClk());
                newPCB.waiting_time = 0; 
                newPCB.last_stop_time = getClk();
                CircularQueue_Enqueue(readyQueue, newPCB);
                printf("At time %d, process %d enqueued with runtime %d\n", getClk(), newPCB.id, newPCB.running_time);
            } else {
                break;
            }
        }

        if (readyQueue->count > 0) {
            PCB currentPCB;
            CircularQueue_Dequeue(readyQueue, &currentPCB);

            int current_time = getClk();

            currentPCB.waiting_time += (current_time - currentPCB.last_stop_time);

            if (currentPCB.pid == -1) {
                int process_pid = fork();
                if (process_pid == 0) {
                    char remaining_time[10];
                    snprintf(remaining_time, sizeof(remaining_time), "%d", currentPCB.remaining_time);
                    execl("process.out", "process.out", remaining_time, NULL);
                    perror("Error in execl");
                    exit(EXIT_FAILURE);
                }
                currentPCB.pid = process_pid;
                currentPCB.start_time = getClk();
                write_schedulerlog_process_started(fptr, getClk(), currentPCB.id, currentPCB.arrival_time,
                                                   currentPCB.running_time, currentPCB.remaining_time,
                                                   currentPCB.waiting_time);
            } else {
                kill(currentPCB.pid, SIGCONT);
                write_schedulerlog_process_resumed(fptr, getClk(), currentPCB.id, currentPCB.arrival_time,
                                                   currentPCB.running_time, currentPCB.remaining_time,
                                                   currentPCB.waiting_time);
            }

            int execTime = (currentPCB.remaining_time > quantum) ? quantum : currentPCB.remaining_time;
            int start_time = getClk();
            while (getClk() - start_time < execTime) { }

            currentPCB.remaining_time -= execTime;

            if (currentPCB.remaining_time <= 0) {
                finalize_pcb(&currentPCB, getClk());
                completed_processes++;

                update_cpu_state(&cpu, currentPCB.weighted_turnaround_time, currentPCB.waiting_time);

                write_schedulerlog_process_finished(fptr, getClk(), currentPCB.id, currentPCB.arrival_time,
                                                    currentPCB.running_time, 0, currentPCB.waiting_time,
                                                    currentPCB.turnaround_time, currentPCB.weighted_turnaround_time);
            } else {
                kill(currentPCB.pid, SIGSTOP);
                currentPCB.last_stop_time = getClk(); 
                write_schedulerlog_process_stopped(fptr, getClk(), currentPCB.id, currentPCB.arrival_time,
                                                   currentPCB.running_time, currentPCB.remaining_time,
                                                   currentPCB.waiting_time);
                CircularQueue_Enqueue(readyQueue, currentPCB);
            }
        } else {
            free_time++;
            sleep(1);
        }
    }

    fclose(fptr);
    int finish_time = getClk();
    finalize_cpu_state(&cpu, num_of_processes, free_time, finish_time);
    write_schedulerprf(cpu.cpu_utilization, cpu.avg_wta, cpu.avg_waiting);

    printf("CPU Utilization: %.2f%%\n", cpu.cpu_utilization);
    printf("Average Waiting Time: %.2f\n", cpu.avg_waiting);
    printf("Average Weighted Turnaround Time: %.2f\n", cpu.avg_wta);
}

