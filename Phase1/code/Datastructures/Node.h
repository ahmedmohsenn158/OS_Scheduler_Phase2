#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../processData.h"


typedef struct Node
{
    PCB Data;
    struct Node *Next;
    int Priority;
} Node;

typedef struct Node_processData
{
    processData Data;
    struct Node_processData *Next;
    int Priority;
} Node_processData;


Node *Create_Pri_Node(PCB item, int Pri)
{
    Node *New_Node = (Node *)malloc(sizeof(Node));
    if (New_Node == NULL)
    {
        return NULL;
    }
    New_Node->Data=item;

    New_Node->Next = NULL;
    New_Node->Priority = Pri;
    return New_Node;
}

Node_processData *Create_Pri_Node_processData(processData item, int Pri)
{
    Node_processData *New_Node = (Node_processData *)malloc(sizeof(Node_processData));
    if (New_Node == NULL)
    {
        return NULL;
    }
    New_Node->Data=item;

    New_Node->Next = NULL;
    New_Node->Priority = Pri;
    return New_Node;
}

//Create_Node & Create_Pri_Node take object instead of pointer (by mohsen)

Node *Create_Node(PCB item)
{
    Node *New_Node = Create_Pri_Node(item, 0);
    return New_Node;
}

Node_processData *Create_Node_processData(processData item)
{
    Node_processData *New_Node = Create_Pri_Node_processData(item, 0);
    return New_Node;
}