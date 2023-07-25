#include "queue.h"
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/time.h>

int count = 0;
int N;
int flag = 1;
volatile int stopThread = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
Queue *queue;
#define BUFFER_SIZE 1024
void *consumer(void *arg);
void *producer(void *arg);
void copyFile(int sourceFile, int destFile);
void copyDirectory(char *sourceDir, char *destDir);
char sourceFile[256];
char destFile[256];
int countDirectory = 0;
int countFile = 0;
void handler(int signal_number)
{
    write(1, "Signal handled\n", 16);
    stopThread = 0;
    free(queue);
}
int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        fprintf(stderr, "Usage error : argc!=5 \n Should be -> ./pCp bufferSize poolSize sourceFile destFile\n");
        return 1;
    }
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &handler;
    sigaction(SIGINT, &sa, NULL);

    memset(sourceFile, '\0', 256);
    memset(destFile, '\0', 256);
    strcpy(sourceFile, argv[3]);
    strcpy(destFile, argv[4]);
    queue = createQueue();
    int poolSize = atoi(argv[2]);
    int bufferSize = atoi(argv[1]);
    N = bufferSize;
    pthread_t *threads = malloc(poolSize * sizeof(pthread_t));
    pthread_t producerThread;
    pthread_create(&producerThread, NULL, producer, NULL);
    for (int i = 0; i < poolSize; i++)
    {
        pthread_create(&threads[i], NULL, consumer, NULL);
    }

    struct timeval currentTime;

    if (gettimeofday(&currentTime, NULL) != 0)
    {
        fprintf(stderr, "gettimeofday error\n");
        return 1;
    }
    long int milliseconds1 = currentTime.tv_sec * 1000 + currentTime.tv_usec / 1000;
    for (int i = 0; i < poolSize; i++)
    {
        pthread_join(threads[i], NULL);
    }
    struct timeval currentTime2;

    if (gettimeofday(&currentTime2, NULL) != 0)
    {
        fprintf(stderr, "gettimeofday error\n");
        return 1;
    }
    long int milliseconds2 = currentTime2.tv_sec * 1000 + currentTime2.tv_usec / 1000;
    fprintf(stdout, "Milliseconds: %ld\n", milliseconds2 - milliseconds1);
    fprintf(stdout, "Total File: %d \nTotal Directory: %d\n", countFile, countDirectory);

    free(threads);
    destroyQueue(queue);
}

void *consumer(void *arg)
{

    while ((flag == 1 || count != 0) && stopThread == 1)
    {
        pthread_mutex_lock(&mutex);

        while (count == 0)
        {
            pthread_cond_wait(&full, &mutex);
        }

        struct FilesInfo filesInfo = dequeue(queue);

        count--;

        pthread_cond_broadcast(&empty);

        pthread_mutex_unlock(&mutex);
        copyFile(filesInfo.sourceFileDesc, filesInfo.destFileDesc);
    }

    return NULL;
}

void *producer(void *arg)
{

    copyDirectory(sourceFile, destFile);
    flag = 0;
}

void copyFile(int sourceFile, int destFile)
{

    if (sourceFile < 0 || destFile < 0)
    {
        perror("File Not Found");
        return;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    while ((bytesRead = read(sourceFile, buffer, BUFFER_SIZE)) > 0)
    {
        write(destFile, buffer, bytesRead);
    }

    close(sourceFile);
    close(destFile);
}
void copyDirectory(char *sourceDir, char *destDir)
{
    DIR *dir = opendir(sourceDir);
    struct dirent *entry;

    if (dir == NULL)
    {
        fprintf(stderr, "sourceDir not opened %s length=%ld\n", sourceDir, strlen(sourceDir));
        exit(0);
    }
    struct stat fileStat;
    if (stat(sourceDir, &fileStat) < 0)
    {
        fprintf(stderr, "Source Directory not opened %s\n", sourceDir);
        exit(0);
    }
    mkdir(destDir, fileStat.st_mode);

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char sourcePath[1024];
        char destPath[1024];

        snprintf(sourcePath, sizeof(sourcePath), "%s/%s", sourceDir, entry->d_name);
        snprintf(destPath, sizeof(destPath), "%s/%s", destDir, entry->d_name);

        if (entry->d_type == DT_REG)
        {
            countFile++;
            pthread_mutex_lock(&mutex);
            while (count == N)
            {
                pthread_cond_wait(&empty, &mutex);
            }

            int sourceFile = open(sourcePath, O_RDONLY);
            int destFile = open(destPath, O_WRONLY | O_CREAT, 0644);
            enqueue(queue, sourceFile, destFile);
            count++;

            pthread_cond_broadcast(&full);
            pthread_mutex_unlock(&mutex);
        }
        else if (entry->d_type == DT_DIR)
        {
            countDirectory++;
            copyDirectory(sourcePath, destPath);
        }
    }
    closedir(dir);
}