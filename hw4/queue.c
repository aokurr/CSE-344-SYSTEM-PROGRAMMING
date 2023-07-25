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

void enqueue(Queue* queue, pid_t pid, const char* arr) {
    if (isFull(queue)) {
        fprintf(stderr,"Queue is full. Cannot enqueue.\n");
        return;
    }

    queue->rear++;
    queue->elements[queue->rear].pid = pid;
    snprintf(queue->elements[queue->rear].arr, sizeof(queue->elements[queue->rear].arr), "%s", arr);
    
    if (isEmpty(queue)) {
        queue->front = 0;
    }
}

struct request dequeue(Queue* queue) {
    struct request frontElement;

    if (isEmpty(queue)) {
        printf("Queue is empty. Cannot dequeue.\n");
        frontElement.pid = -1;
        frontElement.arr[0] = '\0';
        return frontElement;
    }

    frontElement = queue->elements[queue->front];

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

    printf("Queue elements:\n");
    for (int i = queue->front; i <= queue->rear; i++) {
        printf("PID: %d\n", queue->elements[i].pid);
        printf("Array: %s\n", queue->elements[i].arr);
    }
}
