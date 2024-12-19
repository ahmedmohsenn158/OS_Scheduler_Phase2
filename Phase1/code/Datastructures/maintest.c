#include "Queue.h"
#include "CircularQueue.h"
#include"PriQueue.h"
// int main()
// {
//     PriQueue *q = CreatePriQueue();
//     if (q == NULL)
//     {
//         return 1;
//     }
//     // initialising the data with memmory alllocation
//     char *a = (char *)malloc(sizeof(char));
//     char *b = (char *)malloc(sizeof(char));
//     char *c = (char *)malloc(sizeof(char));
//     char *outed = (char *)malloc(sizeof(char));
//     *a = 10;
//     *b = 20;
//     *c = 30;
//     // Enqueue items and making sure the enqueue was succesfull
//     if (!Pri_Enqueue(q, a,10))
//         printf("Failed to enqueue.\n");
//     if (!Pri_Enqueue(q, b,5))
//         printf("Failed to enqueue.\n");
//     if (!Pri_Enqueue(q, c,7))
//         printf("Failed to enqueue.\n");

//     // Dequeue the queue and print it
//     int dummyPri;
//     while (Pri_Dequeue(q, outed,&dummyPri))
//     {
//         printf("Dequeued: %d\n", *outed);
//     }

//     DestroyPriQueue(q);

//     return 0;
// }
int main() {
    CircularQueue q;
    initQueue(&q);

    processData p1 = {1, 0, 5, 3};
    processData p2 = {2, 1, 3, 2};
    processData p3 = {3, 2, 2, 1};

    enqueue(&q, p1);
    enqueue(&q, p2);
    enqueue(&q, p3);
    display(&q);

    dequeue(&q);
    display(&q);

    processData Head = peek(&q);
    if (Head.id != -1) {
        printf("Head process ID: %d\n", Head.id);
    }

    destroyQueue(&q);
    display(&q); // Should show the queue is empty

    return 0;
}
