#include "FifoHeader.h"
#include "queue.h"

static char clientFifo[CLIENT_FIFO_NAME_LEN];

void handler(int signal_number)
{

    unlink(clientFifo);
    exit(0);
}

int logFile(pid_t pid, int clientNumber, int logFileDescriptor, int flag)
{
    char message[128];

    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_CUR;
    fl.l_start = 0;
    fl.l_len = 0;

    if (fcntl(logFileDescriptor, F_SETLK, &fl) == -1)
    {
        fprintf(stderr, "File lock error\n");
        return 1;
    }

    fl.l_type = F_UNLCK;
    if (fcntl(logFileDescriptor, F_SETLK, &fl) == -1)
    {
        fprintf(stderr, "File open lock error\n");
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        fprintf(stderr, "USAGE: %s <Connect/tryConnect> <ServerPID>\n", argv[0]);
        return 1;
    }
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &handler;
    sigaction(SIGINT, &sa, NULL);

    char *option = argv[1];
    int server_pid = atoi(argv[2]);
    int serverFd, clientFd;
    struct request req;
    struct response resp;

    sem_t *clientCountSem;

    umask(0);
    snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMP, (long)getpid());

    if (mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST)
    {
        fprintf(stderr, "ERROR EEXIST\n");
        exit(-1);
    }

    if (strcmp(option, "Connect") == 0)
    {
        req.pid = getpid();
        strcpy(req.arr, "connect\0");
        serverFd = open(SERVER_FIFO, O_WRONLY);

        if (serverFd == -1)
        {
            fprintf(stderr, "open %s\n", SERVER_FIFO);
            exit(-1);
        }
        if (write(serverFd, &req, sizeof(struct request)) != sizeof(struct request))
            perror("Can't write to server1\n");

        clientFd = open(clientFifo, O_RDONLY);
        if (clientFd == -1)
        {
            fprintf(stderr, "open %s\n", clientFifo);
        }
        int clientFd2 = open(clientFifo, O_WRONLY);
        if (clientFd2 == -1)
        {
            fprintf(stderr, "open %s\n", clientFifo);
        }

        if (read(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
            perror("Can't read response from server1\n");

        if (strcmp(resp.arr, "error") == 0)
        {
            fprintf(stdout, "server %d\n", server_pid);
            close(clientFd);
            unlink(clientFifo);
            exit(1);
        }
        clientCountSem = sem_open(CLIENT_MAX, 0);

        if (strcmp(resp.arr, "connected") == 0 || strcmp(resp.arr, "try") == 0)
        {

            fprintf(stdout, "WaitingforQue..\n");
            //sem_wait(clientCountSem);
            while (1)
            {
                fprintf(stdout, "Connection established:\n");
                fprintf(stdout, "Enter Command:\n");
                fgets(req.arr, RESPONSE_LENGTH, stdin);
                req.arr[strlen(req.arr) - 1] = '\0';

                if (strcmp(req.arr, "quit") == 0)
                {
                    fprintf(stdout, "Sending write request to server log file \nwaiting for logfile ...\n");
                }
                if (write(serverFd, &req, sizeof(struct request)) != sizeof(struct request))
                    perror("Can't write to server1\n");

                if (read(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
                    perror("Can't read response from server2\n");
                fprintf(stdout, "%s\n", resp.arr);

                if (strcmp(resp.arr, "quit") == 0)
                {
                    //sem_post(clientCountSem);
                    int value;
                    //sem_getvalue(clientCountSem, &value);
                    fprintf(stdout, "logfile write request\ngrantedbye..value=%d\n",value);
                    close(clientFd);
                    unlink(clientFifo);

                    exit(1);
                }
            }
        }
    }

    return 0;
}
