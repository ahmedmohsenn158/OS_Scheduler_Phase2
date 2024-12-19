#pragma once
#include"Node.h"

typedef struct PriQueue
{
    Node* Head;
    Node* Tail;
    int Count;
} PriQueue;

PriQueue* CreatePriQueue()
{
    PriQueue *q = (PriQueue *)malloc(sizeof(PriQueue));
    if (q == NULL)
    {
        return NULL;
    }
    q->Head=NULL;
    q->Tail=NULL;
    q->Count=0;
    return q;
}

char Pri_Enqueue(PriQueue* q, PCB item, int Pri) 
{
    Node *New_Node = Create_Pri_Node(item, Pri);
    if (New_Node == NULL) {
        return 0; 
    }

    if (q->Head == NULL || New_Node->Priority < q->Head->Priority) {
        New_Node->Next = q->Head;
        q->Head = New_Node;
    } else {
        Node* current = q->Head;
        while (current->Next != NULL && Pri >= current->Next->Priority) {
            current = current->Next;
        }
        New_Node->Next = current->Next;
        current->Next = New_Node;
    }

    q->Count++;

    return 1; 
}



char Pri_Dequeue(PriQueue* q, PCB* item, int* Pri)
{
    if (q->Head == NULL) {
        return 0;
    }

    Node* New_Node = q->Head;
    q->Head = q->Head->Next;  
    if (q->Head == NULL) {
        q->Tail = NULL;
    }

    memcpy(item, &New_Node->Data, sizeof(*item));
    *Pri = New_Node->Priority;
    free(New_Node);
    q->Count--;

    return 1; 
}

char isEmpty(PriQueue* q)
{
    return (q->Head==NULL);

}

// int Peek(PriQueue* q, int* Pri)
// {
//     Node* New_Node = q->Head;
//     *Pri=New_Node->Priority;
//     return &Pri;
// }

void DestroyPriQueue(PriQueue* q) 
{
    Node* current = q->Head;
    while (current != NULL) {
        Node* temp = current;
        current = current->Next;
        //free(temp->Data);
        free(temp);
    }
    free(q);
}

void Pri_Peek(PriQueue* q, int* Pri)
{
    Node* New_Node = q->Head;
    *Pri=New_Node->Priority;
    return ;
}

void displayPriQueue(PriQueue*q) {
    if (q->Count==0) {
        printf("Queue is empty!\n");
        return;
    }

    printf("Queue elements: \n");
    Node *current = q->Head;
    do {
        printf("Process ID: %d\n", current->Data.id);
        current = current->Next;
    } while (current != NULL);
}