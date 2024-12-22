#pragma once
#include "../headers.h"
#define FREE_LIST_SIZE 100

int getRootSize(int size){
    for(int i=1024;i>0;i/=2){
        if (size<=i && size>i/2){
            return i;
        }
    }
    return -1;
}

int isBlockAvailable(int nodesize, Tree* MemoryTree) {
    for (int i = 0; i < MemoryTree->countfree; i++) {
        if (MemoryTree->free[i] != NULL && MemoryTree->free[i]->size == nodesize) {
            return i;
        }
    }
    return -1;
}

int smallestBlockAvailable(Tree * MemoryTree,int process_size){
    int min=1024;
    int min_index=0;
    for (int i=0; i< FREE_LIST_SIZE; i++){
        if (MemoryTree->free[i]!= NULL){
            if (MemoryTree->free[i]->size<min && MemoryTree->free[i]->size >=process_size){
                min=MemoryTree->free[i]->size;
                min_index=i;
            }
        }
    }
    return min_index;
}

int largestBlockAvailable(Tree * MemoryTree){
    int max=0;
    int max_index=0;
    for (int i=0; i< FREE_LIST_SIZE; i++){
        if (MemoryTree->free[i]!= NULL){
            if (MemoryTree->free[i]->size > max){
                max=MemoryTree->free[i]->size;
                max_index=i; 
            }
        }
    }
    return max;
}

void shiftFreeList(Tree* MemoryTree, int startIndex) {
    for (int i = startIndex; i < MemoryTree->countfree - 1; i++) {
        MemoryTree->free[i] = MemoryTree->free[i + 1];
    }
    MemoryTree->free[--MemoryTree->countfree] = NULL;
}

void shiftAllocatedList(Tree* MemoryTree, int startIndex) {
    for (int i = startIndex; i < MemoryTree->countallocated - 1; i++) {
        MemoryTree->allocated[i] = MemoryTree->allocated[i + 1];
    }
    MemoryTree->allocated[--MemoryTree->countallocated] = NULL;
}

void printTreeList(TreeNode** array){
    for (int i=0;i<FREE_LIST_SIZE;i++){
        if (array[i]!=NULL){
            printf("start: %d end: %d size: %d flag: %d process_id: %d\n", 
            array[i]->start,
            array[i]->end,array[i]->size, 
            array[i]->state,
            array[i]->process_id);
        }
    }
}

bool allocateMemoryBlock(int nodesize, Tree* MemoryTree, PCB* pcb) {
    if (MemoryTree == NULL) {
        perror("Invalid MemoryTree structure");
        return false;
    }

    if (MemoryTree->countfree == 0 || getRootSize(pcb->memsize)> largestBlockAvailable(MemoryTree)) {
        printf("No space to allocate for process %d\n", pcb->id);
        return false;
    }

    int i = isBlockAvailable(nodesize, MemoryTree);
    int retries = 0;

    while (i == -1 && retries < FREE_LIST_SIZE) {
        retries++;
        if (MemoryTree->countfree == 0) {
            printf("No space to allocate after %d retries for process %d\n", retries, pcb->id);
            return false;
        }

        int index = smallestBlockAvailable(MemoryTree,pcb->memsize);
        if (index == -1) {
            perror("No smaller block available for splitting");
            return false;
        }

        TreeNode* ptr = MemoryTree->free[index];
        shiftFreeList(MemoryTree, index); 

        createChildren(ptr);

        if (MemoryTree->countfree + 2 > FREE_LIST_SIZE) {
            perror("Free list overflow");
            return false;
        }

        MemoryTree->free[MemoryTree->countfree++] = ptr->left;
        MemoryTree->free[MemoryTree->countfree++] = ptr->right;

        i = isBlockAvailable(nodesize, MemoryTree);
    }

    if (i == -1) {
        printf("Unable to allocate memory for process %d after %d retries\n", pcb->id, retries);
        return false;
    }

    TreeNode* ptr = MemoryTree->free[i];
    shiftFreeList(MemoryTree, i); 

    ptr->process_id = pcb->id;
    ptr->state = 1;
    pcb->start_memory_address=ptr->start;
    MemoryTree-> allocated[MemoryTree->countallocated++] = ptr;

    printf("Allocated Memory for process %d from %d to %d\n", pcb->id, ptr->start, ptr->end);
    return true;
}

void deallocateMemoryBlock(PCB pcb, Tree* MemoryTree) {
    TreeNode* NodePtr = NULL;
    int shift_index = 0;

    for (int i = 0; i < MemoryTree->countallocated; i++) {
        if (MemoryTree->allocated[i] != NULL && MemoryTree->allocated[i]->process_id == pcb.id) {
            NodePtr = MemoryTree->allocated[i];
            MemoryTree->allocated[i] = NULL;
            shift_index = i;
            break;
        }
    }


    printf("Freed %d bytes for process %d from %d to %d\n", pcb.memsize, pcb.id, NodePtr->start, NodePtr->end);
    NodePtr->state = 0;
    shiftAllocatedList(MemoryTree, shift_index);

    while (1) {
        int foundSibling = -1;

        // Find the buddy block in the free list
        for (int i = 0; i < MemoryTree->countfree; i++) {
            if (MemoryTree->free[i] != NULL && MemoryTree->free[i]->start == NodePtr->buddyAddress) {
                foundSibling = i;
                break;
            }
        }

        // If no buddy block is found, add the current block to the free list and exit
        if (foundSibling == -1) {
            MemoryTree->free[MemoryTree->countfree++] = NodePtr;
            break;
        }

        // Merge with buddy block
        TreeNode* BuddyNode = MemoryTree->free[foundSibling];
        TreeNode* Parent = findParent(MemoryTree->root, NodePtr);

        if (Parent == NULL) {
            printf("Error: Parent node not found during merge\n");
            break;
        }

        // Remove buddy from the free list
        MemoryTree->free[foundSibling] = NULL;
        shiftFreeList(MemoryTree, foundSibling);

        // Merge children into the parent
        DeleteChildren(Parent);
        NodePtr = Parent; // Set the parent as the new node to check for further merging
    }
}
