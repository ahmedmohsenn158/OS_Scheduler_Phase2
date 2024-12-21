#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef struct TreeNode{
    int process_id;
    int start;
    int end;
    struct TreeNode* left;
    struct TreeNode* right;
    int state;
    int size;
    
} TreeNode;

typedef struct Tree{
    TreeNode* root;
    TreeNode** free;
    int countfree;
} Tree;

// typedef struct Queue_TreeNode {
//     TreeNode* Head;
//     TreeNode* Tail;
//     int count;
// } Queue_TreeNode;

// typedef struct Node_QueueTree{
//     TreeNode* treenode;
//     TreeNode* next;
// } Node_QueueTree;


TreeNode* CreateNode(int Start, int End, int State){
    TreeNode* node=(TreeNode*)malloc(sizeof(TreeNode));
    node->start=Start;
    node->end=End;
    node->process_id=-1;
    node->left=NULL;
    node->right=NULL;
    node->state=State;
    node->size= End-Start+1;

    return node;
}

Tree* CreateTree() {
    TreeNode* rootnode = CreateNode(0, 1023, 0);

    Tree* MemoryTree = (Tree*)malloc(sizeof(Tree));
    MemoryTree->root = rootnode;

    TreeNode** freeList = (TreeNode**)calloc(100, sizeof(TreeNode*));
    freeList[0]=rootnode;
    for (int i=1;i<20;i++){
        freeList[i]=NULL;
    }

    MemoryTree->free=freeList;
    MemoryTree->countfree=1;

    return MemoryTree;
}

// int Queue_Dequeue_TreeNode(Queue_TreeNode* q, TreeNode* item) 
// {
//     if (q == NULL || q->Head == NULL) {
//         return 0; 
//     }
//     TreeNode* New_Node = q->Head;
//     q->Head = q->Head->Next;
//     if (q->Head == NULL) {
//         q->Tail = NULL;
//     }
//     item = New_Node;
//     free(New_Node);
//     q->count--;
//     return 1;
// }

// int Queue_Enqueue_TreeNode(Queue_TreeNode* q, TreeNode item) {
//     TreeNode* new_node = Create_TreeNode(item);
//     if (new_node == NULL) {
//         return 0; 
//     }
//     if (q->Head == NULL) {
//         q->Head = new_node;
//         q->Tail = new_node;
//     } else {
//         q->Tail->Next = new_node;
//         q->Tail = new_node;
//     }
//     q->count++;
//     return 1; 
// }

void createChildren(TreeNode* parent) {
    if (parent == NULL || parent->size <= 1) return; // No splitting possible

    int half_size = parent->size / 2;

    // Create the left child
    parent->left = malloc(sizeof(TreeNode));
    if (parent->left == NULL) {
        perror("Memory allocation failed for left child");
        return;
    }
    parent->left->start = parent->start;
    parent->left->end = parent->start + half_size - 1;
    parent->left->size = half_size;
    parent->left->state = 0;
    parent->left->left = NULL;
    parent->left->right = NULL;

    // Create the right child
    parent->right = malloc(sizeof(TreeNode));
    if (parent->right == NULL) {
        perror("Memory allocation failed for right child");
        free(parent->left); // Clean up allocated left child
        parent->left = NULL;
        return;
    }
    parent->right->start = parent->start + half_size;
    parent->right->end = parent->end;
    parent->right->size = half_size;
    parent->right->state = 0;
    parent->right->left = NULL;
    parent->right->right = NULL;

    // Mark parent as split
    parent->state = 1;
}

void DeleteChildren(TreeNode*node){
    free(node->right);
    free(node->left);

    node->right=NULL;
    node->left=NULL;
}

void updateState (int Process_id , TreeNode* node){
    node->process_id=Process_id;
    node->state=1;
}

void printTree(TreeNode* root)
{
    if(root==NULL){
        return;
    }
    printf("start: %d end: %d size: %d flag: %d process_id: %d\n", root->start,root->end,root->size,root->state,root->process_id);
    printTree(root->left);
    printTree(root->right);
}

// void DeleteTree(Tree*tree){
//     if(tree->root==NULL){
//         return;
//     }

//     TreeNode* rootPtr=tree->root->right;

// }