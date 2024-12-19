#include "headers.h"

void clearResources(int);

 int msgqueue_id;

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);


    if (argc!=6 &&argc!=4){
        printf("Incorrect number of arguments! Try again! \n");
        exit(-1);
    }

    if (argc==4){
        if (atoi(argv[3])==2||atoi(argv[3])==3){
            printf("You chose Round Robin or Multilevel feedback queue without specifying the quantum\n");
            exit(-1);
        }
    }

    if (atoi(argv[3])!=0 && atoi(argv[3])!=1 && atoi(argv[3])!=2 && atoi(argv[3])!=3){
        printf("Incorrect scheduling algorithm chosen! Choose 0 for SJF, 1 for HPF, 2 for RR, or 3 for MLF\n");
        exit(-1);
    }

    char* file_path = argv[1];

    // TODO Initialization
    // 1. Read the input files.
    FILE * fptr= fopen(file_path,"r");

    if (fptr==NULL){
        printf("Error in opening the file\n");
    }

    Queue* processes_queue= CreateQueue();

    char line[120];
    while (fgets(line,120,fptr)){
        if(line[0]=='#'){
            continue;
        }

        PCB process;
        sscanf(line,"%d %d %d %d", &process.id, &process.arrival_time, &process.running_time, &process.priority);
        int result = Queue_Enqueue(processes_queue, process);    
    }
    fclose(fptr);
    int num_of_processes=processes_queue->count;
    char numofprocesses[3];
    snprintf(numofprocesses,sizeof(numofprocesses), "%d", num_of_processes);

    // 2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.
    int scheduling_algorithm=atoi(argv[3]);
    int quantum=-1;
    if (scheduling_algorithm==2 || scheduling_algorithm==3){
        quantum=atoi(argv[5]);
    }

    // 3. Initiate and create the scheduler and clock processes.
    int clk_pid = fork();
    if (clk_pid < 0) {
        perror("Fork failed for clock");
        exit(1);
    }

    if (clk_pid == 0) {
        printf("Clock process starting...\n");
        printf("I am the clock. My pID is %d and my PPID is %d\n",getpid(),getppid());
        execl("clk.out", "clk.out",NULL);
    }
    initClk();

    // 4. Use this function after creating the clock process to initialize clock.
    // To get time use this function. 
    // int x = getClk();
    // printf("Current Time is %d\n", x);

    int scheduler_pid = fork();
    if (scheduler_pid < 0) {
        perror("Fork failed for scheduler");
        exit(1);
    }

    if (scheduler_pid == 0) {
        printf("Scheduler starting...\n");
        printf("I am the Scheduler. My pID is %d and my PPID is %d\n",getpid(),getppid());
        if (argc==4){
            execl("scheduler.out", "scheduler.out",numofprocesses, argv[3], NULL);
        } else {
            execl("scheduler.out", "scheduler.out", numofprocesses, argv[3], argv[5], NULL);
        }
    }

    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    key_t msgqueue_key;
    int msgqueue_id;

    msgqueue_key= ftok("keypath",'A');

    msgqueue_id=msgget(msgqueue_key, 0666 | IPC_CREAT);

    if(msgqueue_id==-1){
        perror("Error in creating the queue\n");
        exit(-1);
    } else {
        printf("Message queue id=%d\n",msgqueue_id);
    }

    //printf("Message queue count= %d\n\n",processes_queue->count);
    // int x = getClk();
    // printf("Current Time is %d\n", x);
    // TODO Generation Main Loop
    // 6. Send the information to the scheduler at the appropriate time.
    struct msgbuff msg;
    while (processes_queue->count > 0) {
        if (processes_queue->Head->Data.arrival_time == getClk()) {
            Queue_Dequeue(processes_queue, &msg.process);
            msg.mtype = 1;
            if (msgsnd(msgqueue_id, &msg, sizeof(msg.process), !IPC_NOWAIT) == -1) {
                perror("Error sending message");
            }
        }
    }
    
    
    int stat_loc;
    waitpid(clk_pid, &stat_loc, 0);
    waitpid(scheduler_pid, &stat_loc, 0);    
    destroyClk(false);

    printf("Process Generator: All child processes completed.\n");
    

    // 7. Clear clock resources
    // destroyClk(true);
    // int status;
    // waitpid(clk_pid, &status, 0);           
    // waitpid(scheduler_pid, &status, 0);
}

void clearResources(int signum)
{ 
    msgctl(msgqueue_id, IPC_RMID, NULL);
    printf("Process Generator: Cleaning up resources...\n");
    msgctl(msgqueue_id, IPC_RMID, (struct msqid_ds *)0);   // Delete message queue
    destroyClk(true);
    exit(0);
}
