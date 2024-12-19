#pragma once
#include "../headers.h"

int EnqueueInCorrectQueue(Queue_processData **MLFQ, processData process)
{
    if (MLFQ[process.priority] == NULL)
    {
        printf("Queue for priority %d is not initialized.\n", process.priority);
        return -1;
    }
    return Queue_Enqueue_processData(MLFQ[process.priority], process);
}

int IsNotFinished(Queue_processData**MLFQ)
{
    int flag=0;
    for(int i=0;i<11;i++)
    {
        if (MLFQ[i]->count>0)
        {
            flag=1;
        }
    }
    return flag;
}

void PrintAll(Queue_processData**qptr)
{
    for (int i = 0; i < 11; i++)
        {
            if(qptr[i]!=NULL)
            {
                printf("Pri %d ",i);
            QueueDisplay_processData(qptr[i]);
            }
            else
            {
                printf("oh my god e7na fe warta kebeera\n");
                exit(-1);
            }
        }
}

int IsStuck(Queue_processData** q)
{
    int stat=1;
    for(int i=0;i<10;i++)
    {
        if(q[i]->count!=0)
        {
            stat=0;
        }
    }
    return stat;
}

void displayPCB(PCB *process_table,int num)
{
    for(int i=0;i<num;i++)
    {
        printf("  %d  ",process_table[i].id);
    }
    printf("\n");
}

void ImplementMLFQ(int msgqueue_id, int quantum, int num_of_processes, Queue_processData **MLFQ)
{
    printf("here I am starting the MLFQ\n"); // debugging
    int received_processes = 0;              // used to track the number of processes recieved from the message queue
    processData message;                     // used to be recieved and enter the correct queue
    bool running = false;                    // indicates that the first process is recieved
    int LocalRunTime = 0;                    // tracks the runtime cycle
    int freetime = 0;                        // tracks the freetime of the processor
    int SentRunTime = 0;                     // the time to be sent to process.c to run
    int PCBUpdateCounter = 0;                // count of pcn initialised
    processData DequeuedProcess;
    int waitTime=0;
    struct PCB *process_table = malloc(num_of_processes * sizeof(struct PCB));
    if (process_table == NULL) {
        perror("Failed to allocate memory for process_table");
        exit(1);
    }
    for(int i=0;i<num_of_processes;i++)
    {
        process_table[i].id=0;
    }

    FILE* fptr;
    fptr = fopen("scheduler.log.txt", "w");
    if(fptr==NULL)
    {
        printf("couldn't open file \n");
    }
    CPU_State cpu = create_cpu_state();
    PCB pcb;
    while (received_processes < num_of_processes||IsNotFinished(MLFQ))
    {
        while (receive_message_mlf_nowait(msgqueue_id, &pcb) == 1)
        {
            running = true;
            received_processes++;
            message.arrivaltime=pcb.arrival_time;
            message.id=pcb.id;
            message.priority=pcb.priority;
            message.runningtime=pcb.running_time;
                //match dequeued process to the message id arrinal etc...
            if (EnqueueInCorrectQueue(MLFQ, message) == 0)
            {
                printf("Failed to enqueue process %d in queue with priority %d\n", message.id, message.priority);
            }
        }
        printf("At time: %d\n", getClk());
        PrintAll(MLFQ);
        LocalRunTime = 0;
         printf("looping on queues now\n");
        for (int counter = 0; counter < 11; counter++)
        {
            // hena shoof que que
           
            if (MLFQ[counter]->count > 0)
            {
                printf("--------------------------------- Bada2et Teshta8al----------------------- of queue %d\n", counter); // Debuging
                do
                {
                    if (Queue_Dequeue_processData(MLFQ[counter], &DequeuedProcess))
                    {
                        printf("Process is dequeued\n");
                        if (process_table[DequeuedProcess.id-1].id!=DequeuedProcess.id) // to handle if PCB is already initialised
                        {
                            process_table[DequeuedProcess.id-1] = initialize_pcb_RR(DequeuedProcess, getClk());
                            printf("Executing process %d with runtime %d\n", DequeuedProcess.id, DequeuedProcess.runningtime);
                            waitTime=getClk()-process_table[DequeuedProcess.id-1].arrival_time;
                            write_schedulerlog_process_started(fptr, getClk(), process_table[DequeuedProcess.id-1].id, process_table[DequeuedProcess.id-1].arrival_time, process_table[DequeuedProcess.id-1].running_time, process_table[DequeuedProcess.id-1].remaining_time, waitTime);
                        }
                        else
                        {
                            printf("Continuing to Execute process %d with remaining time %d\n", DequeuedProcess.id, process_table[DequeuedProcess.id-1].remaining_time);
                        }
                        if (process_table[DequeuedProcess.id-1].remaining_time < quantum-LocalRunTime) // to set the runtime to be sent to the Process forked
                        {
                            SentRunTime = process_table[DequeuedProcess.id-1].remaining_time;
                        }
                        else
                        {
                            SentRunTime = quantum;
                        }
                        // TODO fork proccess and wait for it to return the remaining time to be given is the smaller between the quantum and running time
                        int process_pid;
                        if(DequeuedProcess.runningtime==process_table[DequeuedProcess.id-1].remaining_time) // to check id the process is running for the first time to fork it 
                        {
                            int process_pid = fork();
                            process_table[DequeuedProcess.id-1].pid=process_pid;
                            if (process_pid < 0)
                            {
                                perror("Error in forking the process");
                                continue;
                            }
                            if (process_pid == 0)
                            {
                                char running_time[10];
                                snprintf(running_time, sizeof(running_time), "%d", DequeuedProcess.runningtime);
                                if(process_table[DequeuedProcess.id-1].remaining_time==process_table[DequeuedProcess.id-1].running_time)
                                {
                                    execl("process.out", "process.out", running_time, NULL);
                                }
                                perror("Error in execlp");
                                exit(EXIT_FAILURE);
                            }
                            else
                            {
                                //sleep(SentRunTime);
                                int sleep=getClk()+SentRunTime;
                                while(getClk()<sleep){}
                                kill(process_pid, SIGSTOP); // actualy stops the process
                                process_table[DequeuedProcess.id-1].remaining_time -= SentRunTime; // decrementing the remaining run time
                                printf("A process has been stopped at time %d with remaining time %d\n", getClk(), process_table[DequeuedProcess.id-1].remaining_time);
                            }
                        }
                        else // the process ran before so will not fork will SIGCONT to continue only
                        {
                            printf("the process is existing will continue its Starting Time is %d\n",getClk());
                            waitTime = getClk() - process_table[DequeuedProcess.id-1].arrival_time - process_table[DequeuedProcess.id-1].running_time + process_table[DequeuedProcess.id-1].remaining_time;
                            write_schedulerlog_process_resumed(fptr, getClk(), process_table[DequeuedProcess.id-1].id, process_table[DequeuedProcess.id-1].arrival_time, process_table[DequeuedProcess.id-1].running_time, process_table[DequeuedProcess.id-1].remaining_time, waitTime);
                            int stat_loc;
                            kill(process_table[DequeuedProcess.id-1].pid, SIGCONT);
                            //sleep(SentRunTime);
                            int sleep=getClk()+SentRunTime;
                            while(getClk()<sleep){}
                            kill(process_table[DequeuedProcess.id-1].pid, SIGSTOP);
                            process_table[DequeuedProcess.id-1].remaining_time -= SentRunTime; // decrementing the remaining run time
                            printf("A process has been stopped at time %d with remaining time %d\n", getClk(), process_table[DequeuedProcess.id-1].remaining_time);
                        }
                        if (process_table[DequeuedProcess.id-1].remaining_time > 0) // to return the unfinished process to the queue in the lower level
                        {
                            printf("the process did not finish and is geting relegated\n");
                            waitTime = getClk() - process_table[DequeuedProcess.id-1].arrival_time - process_table[DequeuedProcess.id-1].running_time + process_table[DequeuedProcess.id-1].remaining_time;
                            write_schedulerlog_process_Stoped(fptr, getClk(), process_table[DequeuedProcess.id-1].id, process_table[DequeuedProcess.id-1].arrival_time, process_table[DequeuedProcess.id-1].running_time, process_table[DequeuedProcess.id-1].remaining_time, waitTime);
                            if(counter==10)
                            {
                                Queue_Enqueue_processData(MLFQ[counter], DequeuedProcess);
                            }
                            else
                            {
                                printf("relegating process\n");
                                Queue_Enqueue_processData(MLFQ[counter + 1], DequeuedProcess);
                            }
                            if(IsStuck(MLFQ)&&counter==10&&MLFQ[10]->count!=0)
                            {
                                processData temp;
                                int numero=MLFQ[counter]->count;
                                for(int i=0;i<numero;i++)
                                {
                                    Queue_Dequeue_processData(MLFQ[10],&temp);
                                    EnqueueInCorrectQueue(MLFQ,temp);
                                }
                            }
                        }
                        else
                        {
                            PCB* temp=&process_table[DequeuedProcess.id-1];
                            printf("the process did finish and is geting finalised\n");
                            finalize_pcb(temp ,getClk());
                            waitTime = getClk() - process_table[DequeuedProcess.id-1].arrival_time - process_table[DequeuedProcess.id-1].running_time + process_table[DequeuedProcess.id-1].remaining_time;
                            update_cpu_state(&cpu, process_table[DequeuedProcess.id-1].weighted_turnaround_time, waitTime);                              
                            write_schedulerlog_process_finished(fptr, getClk(), process_table[DequeuedProcess.id-1].id, process_table[DequeuedProcess.id-1].arrival_time, process_table[DequeuedProcess.id-1].running_time, process_table[DequeuedProcess.id-1].remaining_time, waitTime, process_table[DequeuedProcess.id-1].turnaround_time, process_table[DequeuedProcess.id-1].weighted_turnaround_time);                         
                        }
                        LocalRunTime += SentRunTime;
                    }
                    else
                    {
                        break;
                    }
                    PCBUpdateCounter++;
                } while (LocalRunTime < quantum && LocalRunTime != 0);
            }
            if (LocalRunTime>=quantum)
            {
                break;
            }
        }
        if (LocalRunTime == 0)
        {
            printf("++++++++++++++++++no processes to excite 1 sec idle+++++++++++++++++++++++++++++++++++\n"); //debugging
            freetime++;
            sleep(1);
        }
    }
    int finish_time = getClk();
    freetime++;
    printf("All processes completed, producing the output file \n");
    printf("the freetime is %d\n",freetime);
    printf("--------------------------------------------------------------------------------------------");
    if(fptr!=NULL)
    {
        fclose(fptr);
    }
    finalize_cpu_state(&cpu, num_of_processes, freetime, finish_time);
    write_schedulerprf(cpu.cpu_utilization, cpu.avg_wta, cpu.avg_waiting);
    free(process_table);
}