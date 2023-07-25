#ifndef QUEUE_H
#define QUEUE_H
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_SIZE 1024

struct FilesInfo{
    int sourceFileDesc; 
    int destFileDesc;
};

typedef struct {
    struct FilesInfo fileBuffer[MAX_SIZE];
    int front;
    int rear;
} Queue;

Queue* createQueue();
int isEmpty(Queue* queue);
int isFull(Queue* queue);
void enqueue(Queue* queue, int sourceFileDesc , int destFileDesc);
struct FilesInfo dequeue(Queue* queue);
void destroyQueue(Queue* queue);
void displayQueue(Queue* queue);
#endif
