#include "serverLib.h"
#include "queue.h"
int logFileDescriptor;
int clientNumber;
int clientTable[256];
void handler(int signal_number)
{   
    for (int i = 0; i < clientNumber; i++)
    {   
         kill(clientTable[i], SIGINT);
        
    }
    close(logFileDescriptor);
    unlink(SERVER_FIFO);
    exit(0);
}
int count = 0;
int N = 10;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
Queue *queue;
void *consumer(void *arg);
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: argc!=4\n");
        return 1;
    }
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &handler;
    sigaction(SIGINT, &sa, NULL);

    char *dirname = argv[1];
    int max_clients = atoi(argv[2]);
    int poolSize = atoi(argv[3]);
    int serverFd, dummyFd;

    sem_t *logSem;
    sem_t *clientCountSem;
    
    clientCountSem = sem_open(CLIENT_MAX, O_CREAT, 0666, 0);
    sem_init(clientCountSem, 0, max_clients);
    clientNumber=0;
    struct request req;
    queue = createQueue();
    umask(0);

    if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST)
    {
        fprintf(stderr, "mkfifo error\n");
        exit(1);
    }

    pthread_t *threads = malloc(poolSize * sizeof(pthread_t));

    for (int i = 0; i < poolSize; i++)
    {
        int *thread_number = malloc(sizeof(int));
        *thread_number = i + 1;
        pthread_create(&threads[i], NULL, consumer, (void *)thread_number);
    }

    fprintf(stdout, "Server Started PID  %d\n", getpid());
    fprintf(stdout, "waiting for clients..\n");

    logFileDescriptor = open("log.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (logFileDescriptor == -1)
    {
        fprintf(stderr, "Log File Error\n");
        return 1;
    }

    logFile(getpid(), 0, logFileDescriptor, 0);
    logSem = sem_open(LOG_FILE_SEM, O_CREAT, 0666, 0);

    sem_post(logSem);

    serverFd = open(SERVER_FIFO, O_RDONLY);
    if (serverFd == -1)
    {
        fprintf(stderr, "open server error\n");
        exit(1);
    }
    dummyFd = open(SERVER_FIFO, O_WRONLY);

    if (dummyFd == -1)
    {
        fprintf(stderr, "open dummy file\n");
        exit(1);
    }

    while (1)
    {
        if (read(serverFd, &req, sizeof(struct request)) != sizeof(struct request))
        {
            fprintf(stderr, "error reading1\n");
            continue;
        }
        pthread_mutex_lock(&mutex);

        while (count == N)
        {
            pthread_cond_wait(&empty, &mutex);
        }
        enqueue(queue, req.pid, req.arr);
        count++;

        pthread_cond_broadcast(&full);

        pthread_mutex_unlock(&mutex);

    }
    close(logFileDescriptor);
    free(threads);
    unlink(SERVER_FIFO);
}

void *consumer(void *arg)
{
    char clientFifo[CLIENT_FIFO_NAME_LEN];
    int clientFd;
    int i;
    while (1)
    {
        pthread_mutex_lock(&mutex);

        while (count == 0)
        {
            pthread_cond_wait(&full, &mutex);
        }
        int thread_number = *((int *)arg);

        struct request element = dequeue(queue);
        for(i = 0; i < clientNumber; i++){
            if(clientTable[i]==element.pid)
                break;
        }
        if(i==clientNumber){
            clientTable[clientNumber++]=element.pid;
        }

        if(strcmp(element.arr,"quit")==0){
            logFile(element.pid, i, logFileDescriptor, 2);
        }
        else if(strcmp(element.arr,"connect")==0){
            logFile(element.pid, i, logFileDescriptor, 1);
        }

        
        count--;

        pthread_cond_broadcast(&empty);

        pthread_mutex_unlock(&mutex);

        snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMP, (long)element.pid);

        clientFd = open(clientFifo, O_WRONLY);

        handle_client(element.arr, element.pid, clientFd);
        close(clientFd);
    }

    return NULL;
}