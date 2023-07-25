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


#define LOG_FILE_SEM "logFile"
#define CLIENT_MAX "semmax"

#define CLIENT_FIFO_NAME_LEN (sizeof(CLIENT_FIFO_TEMP) + 20)
#define RESPONSE_LENGTH 1024


struct response
{
    char arr[256];

};