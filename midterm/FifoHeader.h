#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <semaphore.h>
#include <errno.h>
#include <signal.h>
#define SERVER_FIFO "/tmp/bibo_fifo_sv"
#define CLIENT_FIFO_TEMP "/tmp/bibo_fifo_cl.%ld"

#define CLIENT_SEM_TEMP "sem.%ld"
#define CLIENT_SEM2_TEMP "sem2.%ld"

#define LOG_FILE_SEM "sem3"
#define CLIENT_SEM_QUEUE "semQueue"
#define CLIENT_FIFO_NAME_LEN (sizeof(CLIENT_FIFO_TEMP) + 20)
#define CLIENT_SEM_NAME_LEN (sizeof(CLIENT_SEM_TEMP) + 20)
#define RESPONSE_LENGTH 1024*5
#define MAX_CLIENTS 20

int handle_client(char *buffer, pid_t pid, int clientFd);
struct request
{
    pid_t pid;
    pid_t serverPid;
    char arr[256];
};
struct response
{
    char arr[RESPONSE_LENGTH];

};