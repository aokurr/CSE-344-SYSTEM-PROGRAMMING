#ifndef QUEUE_H
#define QUEUE_H
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_SIZE 1024


typedef struct {
    int buffer[MAX_SIZE];
    int front;
    int rear;
} Queue;

Queue* createQueue();
int isEmpty(Queue* queue);
int isFull(Queue* queue);
void enqueue(Queue* queue, int item);
int dequeue(Queue* queue);
void destroyQueue(Queue* queue);
void displayQueue(Queue* queue);
#endif
