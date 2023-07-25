#include "queue.h"
Queue *createQueue()
{
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    queue->front = -1;
    queue->rear = -1;
    return queue;
}

int isEmpty(Queue *queue)
{
    return queue->front == -1;
}

int isFull(Queue *queue)
{
    return queue->rear == MAX_SIZE - 1;
}

void enqueue(Queue *queue, int item)
{
    if (isFull(queue))
    {
        fprintf(stderr, "Queue is full. Cannot enqueue.\n");
        return;
    }

    queue->rear++;
    queue->buffer[queue->rear] = item;

    if (isEmpty(queue))
    {
        queue->front = 0;
    }
}

int dequeue(Queue *queue)
{   

    if (isEmpty(queue))
    {
        printf("Queue is empty. Cannot dequeue.\n");
        
        return -1;
    }

    int frontElement = queue->buffer[queue->front];

    if (queue->front == queue->rear)
    {
        queue->front = -1;
        queue->rear = -1;
    }
    else
    {
        queue->front++;
    }

    return frontElement;
}
void destroyQueue(Queue *queue)
{
    free(queue);
}
void displayQueue(Queue *queue)
{
    if (isEmpty(queue))
    {
        printf("Queue is empty.\n");
        return;
    }

    printf("Queue buffer:\n");
    for (int i = queue->front; i <= queue->rear; i++)
    {
        printf("item: %d\n", queue->buffer[i]);
    }
}
