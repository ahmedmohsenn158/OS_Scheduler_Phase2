
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "Node.h"


typedef struct CircularQueue
{
    Node* Tail;
    int count;
} CircularQueue;

CircularQueue * CreateCircularQueue()
{
    CircularQueue *q = (CircularQueue *)malloc(sizeof(CircularQueue));
    if (q == NULL)
    {
        return NULL;
    }
    q->Tail = NULL;
    q->count = 0;
    return q;
}

char CircularQueue_Enqueue(CircularQueue *q, PCB item)
{
    Node *New_Node = Create_Node(item);
    if (New_Node == NULL)
    {
        return 0;
    }

    if (q->Tail == NULL)
    {
        New_Node->Next = New_Node;  
        q->Tail = New_Node;
    }
    else
    {
        New_Node->Next = q->Tail->Next;
        q->Tail->Next = New_Node;
        q->Tail = New_Node;
    }
    q->count++;
    return 1;
}

// char CircularQueue_Dequeue(CircularQueue *q, processData *item)
// {
//     Node *New_Node = q->Tail->Next;
//     if (New_Node == NULL)
//     {
//         return 0;
//     }
//     printf("created node\n");
//     if (q->Tail == NULL)
//     {
//         return 0;
//     }
//     else if (q->Tail == q->Tail->Next)
//     {
//         printf("uding the given item\n");
//         *item=q->Tail->Data;
//         // q->Tail->Data = NULL;
//         q->Tail = NULL;
//         printf("used the given item\n");
//     }
//     else
//     {
//         printf("uding the given item\n");
//         q->Tail->Next = New_Node->Next;
//         *item=New_Node->Data;
//         free(New_Node);
//         printf("used the given item\n");
//     }
//     q->count--;
//     return 1;
// }

// char CircularQueuePeek(CircularQueue* q, processData* item)
// {
//     if (q->Tail == NULL)
//     {
//         return 0;
//     }
//     *item=q->Tail->Next->Data;
//     return 1;
// }


char CircularQueue_Dequeue(CircularQueue *q, PCB *item) {
    if (q == NULL || q->Tail == NULL) {
        return 0; // Queue is NULL or empty
    }

    Node *New_Node = q->Tail->Next;
    if (New_Node == NULL) {
        return 0; // Inconsistent state
    }

    if (q->Tail == q->Tail->Next) { // Only one element in the queue
        *item = q->Tail->Data;
        free(q->Tail);
        q->Tail = NULL;
    } else { // More than one element
        q->Tail->Next = New_Node->Next;
        *item = New_Node->Data;
        free(New_Node);
    }

    if (q->count > 0) {
        q->count--;
    }

    return 1; // Dequeue successful
}


 void DestroyCircularQueue(CircularQueue *q)
{
    Node *current = q->Tail;
    while (current != NULL)
    {
        Node *temp = current;
        current = current->Next;
        free(temp);
    }
    free(q);
}


// processData peek(CircularQueue *q) {
//     processData value;
//     if (Circular_isEmty(q)) {
//         printf("Queue is empty! Nothing to peek.\n");
//         value.id = -1; 
//         return value;
//     }
//     value = q->Head->Data;
//     return value;
// }

// void CircularQueueDisplay(CircularQueue*q) {
//     if (q->count==0) {
//         printf("Queue is empty!\n");
//         return;
//     }

//     printf("Queue elements: \n");
//     Node *current = q->Tail->Next;
//     do {
//         printf("Process ID: %d\n", current->Data.id);
//         current = current->Next;
//     } while (current != q->Tail->Next);
// }

void CircularQueueDisplay(CircularQueue *q) {
    if (q == NULL) {
        printf("Queue is uninitialized!\n");
        return;
    }

    if (q->count == 0 || q->Tail == NULL) {
        printf("Queue is empty!\n");
        return;
    }

    printf("Queue elements:\n");
    Node *current = q->Tail->Next; // Start from the head of the queue
    int printed = 0;              // Safety mechanism to prevent infinite loops
    do {
        if (current == NULL) {    // Check for corruption
            printf("Corrupted queue detected!\n");
            return;
        }

        printf("Process ID: %d\n", current->Data.id);
        current = current->Next;
        printed++;

        if (printed > q->count) { // Detect circular reference issues
            printf("Corrupted queue detected (infinite loop)!\n");
            return;
        }
    } while (current != q->Tail->Next);
}


// void DestroyCircularQueue(CircularQueue *q) {
//     while (!Circular_isEmty(q)) {
//         dequeue(q);
//     }
// }