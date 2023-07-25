#include "queue.h"
#include <sys/types.h>
Queue* createQueue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->front = -1;
    queue->rear = -1;
    return queue;
}

int isEmpty(Queue* queue) {
    return queue->front == -1;
}

int isFull(Queue* queue) {
    return queue->rear == MAX_SIZE - 1;
}

void enqueue(Queue* queue,int sourceFileDesc,int destFileDesc ) {
    if (isFull(queue)) {
        fprintf(stderr,"Queue is full. Cannot enqueue.\n");
        return;
    }

    queue->rear++;
    queue->fileBuffer[queue->rear].sourceFileDesc = sourceFileDesc;
    queue->fileBuffer[queue->rear].destFileDesc = destFileDesc;
    
    if (isEmpty(queue)) {
        queue->front = 0;
    }
}

struct FilesInfo dequeue(Queue* queue) {
    struct FilesInfo frontElement;

    if (isEmpty(queue)) {
        printf("Queue is empty. Cannot dequeue.\n");
        frontElement.sourceFileDesc = -1;
        frontElement.destFileDesc = -1;
        return frontElement;
    }

    frontElement = queue->fileBuffer[queue->front];

    if (queue->front == queue->rear) {
        queue->front = -1;
        queue->rear = -1;
    } else {
        queue->front++;
    }

    return frontElement;
}
void destroyQueue(Queue* queue) {
    free(queue);
}
void displayQueue(Queue* queue) {
    if (isEmpty(queue)) {
        printf("Queue is empty.\n");
        return;
    }

    printf("Queue fileBuffer:\n");
    for (int i = queue->front; i <= queue->rear; i++) {
        printf("sourceFileDesc: %d\n", queue->fileBuffer[i].sourceFileDesc);
        printf("destFileDesc: %d\n", queue->fileBuffer[i].destFileDesc);
    }
}
