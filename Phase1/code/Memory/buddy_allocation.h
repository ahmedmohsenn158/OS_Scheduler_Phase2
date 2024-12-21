#pragma once
#include "../headers.h"

int getRootSize(int size){
    for(int i=1024;i>0;i/=2){
        if (size<=i && size>i/2){
            return i;
        }
    }
}

#define FREE_LIST_SIZE 100

int isBlockAvailable(int nodesize, Tree* MemoryTree) {
    for (int i = 0; i < FREE_LIST_SIZE; i++) {
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

void printFreeList(Tree*MemoryTree){
    for (int i=0;i<FREE_LIST_SIZE;i++){
        if (MemoryTree->free[i]!=NULL){
            printf("start: %d end: %d size: %d flag: %d process_id: %d\n", 
            MemoryTree->free[i]->start,
            MemoryTree->free[i]->end,MemoryTree->free[i]->size, 
            MemoryTree->free[i]->state,
            MemoryTree->free[i]->process_id);
        }
    }
}

bool allocateMemoryBlock(int nodesize, Tree* MemoryTree, PCB pcb) {
    if (MemoryTree == NULL) {
        perror("Invalid MemoryTree structure");
        return false;
    }

    if (MemoryTree->countfree == 0 || getRootSize(pcb.memsize)> largestBlockAvailable(MemoryTree)) {
        printf("No space to allocate for process %d\n", pcb.id);
        return false;
    }

    int i = isBlockAvailable(nodesize, MemoryTree);
    int retries = 0;

    while (i == -1 && retries < FREE_LIST_SIZE) {
        retries++;
        if (MemoryTree->countfree == 0) {
            printf("No space to allocate after %d retries for process %d\n", retries, pcb.id);
            return false;
        }

        int index = smallestBlockAvailable(MemoryTree,pcb.memsize);
        if (index == -1) {
            perror("No smaller block available for splitting");
            return false;
        }

        TreeNode* ptr = MemoryTree->free[index];
        shiftFreeList(MemoryTree, index); // Remove block from free list

        createChildren(ptr);

        if (MemoryTree->countfree + 2 > FREE_LIST_SIZE) {
            perror("Free list overflow");
            return false;
        }

        // Add new children to the free list
        MemoryTree->free[MemoryTree->countfree++] = ptr->left;
        MemoryTree->free[MemoryTree->countfree++] = ptr->right;

        i = isBlockAvailable(nodesize, MemoryTree);
    }

    if (i == -1) {
        printf("Unable to allocate memory for process %d after %d retries\n", pcb.id, retries);
        return false;
    }

    // Allocate the block
    TreeNode* ptr = MemoryTree->free[i];
    shiftFreeList(MemoryTree, i); // Remove block from free list

    ptr->process_id = pcb.id;
    ptr->state = 1;

    printf("Allocated Memory for process %d from %d to %d\n", pcb.id, ptr->start, ptr->end);
    return true;
}