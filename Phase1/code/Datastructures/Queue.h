#pragma once
#include <stdio.h>
#include <stdlib.h>
#include"Node.h"

typedef struct Queue {
    Node* Head;
    Node* Tail;
    int count;
} Queue;

typedef struct Queue_processData {
    Node_processData* Head;
    Node_processData* Tail;
    int count;
} Queue_processData;


Queue* CreateQueue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    if (q == NULL) {
        return NULL;
    }
    q->Head = NULL;
    q->Tail = NULL;
    q->count = 0;
    return q;
}

Queue_processData* CreateQueue_processData() {
    Queue_processData* q = (Queue_processData*)malloc(sizeof(Queue_processData));
    if (q == NULL) {
        return NULL;
    }
    q->Head = NULL;
    q->Tail = NULL;
    q->count = 0;
    return q;
}

char Queue_Enqueue(Queue* q, PCB item) {
    Node* new_node = Create_Node(item);
    if (new_node == NULL) {
        return 0; 
    }

    if (q->Head == NULL) {
        q->Head = new_node;
        q->Tail = new_node;
    } else {
        q->Tail->Next = new_node;
        q->Tail = new_node;
    }

    q->count++;
    return 1; 
}

char Queue_Enqueue_processData(Queue_processData* q, processData item) {
    Node_processData* new_node = Create_Node_processData(item);
    if (new_node == NULL) {
        return 0; 
    }

    if (q->Head == NULL) {
        q->Head = new_node;
        q->Tail = new_node;
    } else {
        q->Tail->Next = new_node;
        q->Tail = new_node;
    }

    q->count++;
    return 1; 
}

int Queue_Dequeue(Queue* q, PCB* item) 
{
    if (q == NULL || q->Head == NULL) {
        return 0; 
    }

    Node* New_Node = q->Head;

    q->Head = q->Head->Next;

    if (q->Head == NULL) {
        q->Tail = NULL;
    }

    *item = New_Node->Data;

    free(New_Node);

    q->count--;

    return 1;
}

int Queue_Dequeue_processData(Queue_processData* q, processData* item) 
{
    if (q == NULL || q->Head == NULL) {
        return 0; 
    }

    Node_processData* New_Node = q->Head;

    q->Head = q->Head->Next;

    if (q->Head == NULL) {
        q->Tail = NULL;
    }

    *item = New_Node->Data;

    free(New_Node);

    q->count--;

    return 1;
}

void QueueDisplay(Queue *q) {
    // Check if the queue is uninitialized
    if (q == NULL) {
        printf("Queue is uninitialized!\n");
        return;
    }

    // Check if the queue is empty
    if (q->count == 0 || q->Head == NULL) {
        printf("Queue is empty!\n");
        return;
    }

    printf("Queue elements:\n");

    Node *current = q->Head;
    int printed = 0; 

    while (current != NULL) {
        printf("Process ID: %d\n", current->Data.id);
        current = current->Next;
        printed++;

        if (printed > q->count) {
            printf("Corrupted queue detected (node count exceeds expected count)!\n");
            return;
        }
    }

    if (printed != q->count) {
        printf("Warning: Queue count mismatch (expected %d, found %d)!\n", q->count, printed);
    }
}

void QueueDisplay_processData(Queue_processData *q) {
    // Check if the queue is uninitialized
    if (q == NULL) {
        printf("Queue is uninitialized!\n");
        return;
    }

    // Check if the queue is empty
    if (q->count == 0 || q->Head == NULL) {
        printf("Queue is empty!\n");
        return;
    }

    printf("Queue elements:\n");

    Node_processData *current = q->Head;
    int printed = 0; 

    while (current != NULL) {
        printf("Process ID: %d\n", current->Data.id);
        current = current->Next;
        printed++;

        if (printed > q->count) {
            printf("Corrupted queue detected (node count exceeds expected count)!\n");
            return;
        }
    }

    if (printed != q->count) {
        printf("Warning: Queue count mismatch (expected %d, found %d)!\n", q->count, printed);
    }
}


void DestroyQueue(Queue* q) {
    Node* current = q->Head;
    while (current != NULL) {
        Node* temp = current;
        current = current->Next;
        //free(temp->Data);
        free(temp);
    }
    free(q);
}

void DestroyQueue_processData(Queue_processData* q) {
    Node_processData* current = q->Head;
    while (current != NULL) {
        Node_processData* temp = current;
        current = current->Next;
        //free(temp->Data);
        free(temp);
    }
    free(q);
}
