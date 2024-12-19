#include "headers.h"
#include"SchedulerAlgorithms/SJF.h"
#include"SchedulerAlgorithms/MLF.h"
#include"SchedulerAlgorithms/HPF.h"
#include"SchedulerAlgorithms/RR.h"

PCB receive_message(int msgqueue_id){
    struct msgbuff message;
    int receive_value=msgrcv(msgqueue_id,&message, sizeof(message.process),0,!IPC_NOWAIT);
    if (receive_value==-1){
        perror("Error in receiving the message\n");
        exit(-1);
    } else {
        printf("Process %d with the following arrival time received successfully: %d \n",message.process.id,message.process.arrival_time);
        return message.process;
    }
}
int initialize_message_queue(){
    key_t msgqueue_key;
    int msgqueue_id;

    msgqueue_key= ftok("keypath",'A');

    msgqueue_id=msgget(msgqueue_key, 0666 | IPC_CREAT);

    if (msgqueue_id==-1){
        perror("Error in creating the queue\n");
        exit(-1);
    }

    return msgqueue_id;
}

int MessageQueueIsEmpty(int mID) // Checks id the MessageQueue is emty and returns 1 if it is emty 0 if not
{
    struct msqid_ds msgQueueInfo;
    if (msgctl(mID, IPC_STAT, &msgQueueInfo) == -1)
    {
        perror("Error getting message queue info");
        exit(1);
    }
    return msgQueueInfo.msg_qnum == 0;
}

int main(int argc, char *argv[])
{
    initClk();
    
    int scheduling_algorithm=atoi(argv[2]);
    int quantum=-1;
    if (argc>3){
        quantum=atoi(argv[3]);
    }
    int num_of_processes=atoi(argv[1]);

    int msgqueue_id=initialize_message_queue();
    PriQueue* HPF = CreatePriQueue();

    switch(scheduling_algorithm){
        case 0:{
            PriQueue* SJFQueue=CreatePriQueue();
            if (SJFQueue == NULL) {
                printf("Failed to create priority queue.\n");
                return -1;
            }
            Execute_SJF(num_of_processes,msgqueue_id,SJFQueue);
            DestroyPriQueue(SJFQueue);
            break;
        }

        case(1):{
            PriQueue* HPFQueue=CreatePriQueue();
            if (HPFQueue == NULL) {
                printf("Failed to create priority queue.\n");
                return -1;
            }
            HPF_Execute(HPFQueue, msgqueue_id, num_of_processes);
            DestroyPriQueue(HPFQueue);
            break;
        }

        case 2: 
        {
                CircularQueue *RRQueue = CreateCircularQueue();
                int quantum = atoi(argv[3]); // Quantum passed as a command-line argument
                Execute_RR(num_of_processes, msgqueue_id, RRQueue, quantum);
                DestroyCircularQueue(RRQueue);
                break;
        }

        case(3):{
            int size=11;
            Queue_processData** RRQueues = (Queue_processData**)malloc(size * sizeof(Queue_processData*));

            for (int i=0;i<size;i++){
                RRQueues[i]=CreateQueue_processData();
            }
            ImplementMLFQ(msgqueue_id,quantum,num_of_processes,RRQueues);

            for(int i=0;i<size;i++){
                DestroyQueue_processData(RRQueues[i]);
            }
            free(RRQueues);
            break;
        }

        default:
        {
            break;
        }
    }

   destroyClk(true);
}