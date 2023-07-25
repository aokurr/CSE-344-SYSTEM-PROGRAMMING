#ifndef QUEUE_H
#define QUEUE_H
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_SIZE 1024

struct request{
    pid_t pid;
    char arr[1024];
};

typedef struct {
    struct request elements[MAX_SIZE];
    int front;
    int rear;
} Queue;

Queue* createQueue();
int isEmpty(Queue* queue);
int isFull(Queue* queue);
void enqueue(Queue* queue, pid_t pid, const char* arr);
struct request dequeue(Queue* queue);
void destroyQueue(Queue* queue);
void displayQueue(Queue* queue);
#endif
