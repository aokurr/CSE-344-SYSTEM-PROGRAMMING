
#include "FifoHeader.h"

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
    char clientSem[CLIENT_SEM_NAME_LEN];
    sem_t *sema;
    char clientSem2[CLIENT_SEM_NAME_LEN];
    sem_t *sema2;
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
        req.serverPid = server_pid;

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

        if (read(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
            perror("Can't read response from server1\n");
        if (strcmp(resp.arr, "error") == 0)
        {
            fprintf(stdout, "server %d\n", server_pid);
            close(clientFd);
            unlink(clientFifo);
            exit(1);
        }

        snprintf(clientSem, CLIENT_SEM_NAME_LEN, CLIENT_SEM_TEMP, (long)getpid());
        sema = sem_open(clientSem, 0);

        snprintf(clientSem2, CLIENT_SEM_NAME_LEN, CLIENT_SEM2_TEMP, (long)getpid());

        sema2 = sem_open(clientSem2, 0);

        if (strcmp(resp.arr, "connected") == 0 || strcmp(resp.arr, "try") == 0)
        {
            if (strcmp(resp.arr, "try") == 0)
            {
                fprintf(stdout, "Waiting for Que..\n");
            }
            else
            {
                fprintf(stdout, "Waiting for Que.. Connection established:\n");
            }
            int clientFd2 = open(clientFifo, O_WRONLY);

            while (1)
            {
                fprintf(stdout, "Enter Command:\n");
                fgets(resp.arr,RESPONSE_LENGTH,stdin);     
                resp.arr[strlen(resp.arr)-1] = '\0';

                if (strcmp(resp.arr, "quit") == 0)
                {
                    fprintf(stdout, "Sending write request to server log file \nwaiting for logfile ...\n");
                }
                if (write(clientFd2, &resp, sizeof(struct response)) != sizeof(struct response))
                    perror("Can't write to server2\n");

                if (clientFd2 == -1)
                {
                    fprintf(stderr, "open %s\n", clientFifo);
                    exit(-1);
                }
                sem_wait(sema);
                int size = read(clientFd, &resp, sizeof(struct response));
                sem_post(sema2);
                fprintf(stdout, "%s\n", resp.arr);
                if (strcmp(resp.arr, "quit") == 0)
                {
                    fprintf(stdout, "logfile write request\ngrantedbye..\n");
                    close(clientFd);
                    close(clientFd2);
                    unlink(clientFifo);
                    exit(1);
                }
            }
        }
    }

    return 0;
}
