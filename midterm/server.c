#include "FifoHeader.h"
#include <dirent.h>
int logFile(pid_t pid, int clientNumber, int logFileDescriptor, int flag);
char *helpRequest(char *arr);
int readFile(const char *filename, int line, char *resp);
int writeFile(const char *filename, int lineNumber, const char *str);
int logFileDescriptor;
int clientNumber = 0;
pid_t forkTable[256];
pid_t clientTable[256];
void handler(int signal_number)
{
    for (int i = 0; i < clientNumber; i++)
    {
        if (clientTable[i] != -1)
        {
            kill(clientTable[i], SIGINT);
            kill(forkTable[i], SIGINT);
        }
    }
    close(logFileDescriptor);
    unlink(SERVER_FIFO);
    exit(0);
}
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: argc!=3\n");
        return 1;
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &handler;
    sigaction(SIGINT, &sa, NULL);

    char *dirname = argv[1];
    int max_clients = atoi(argv[2]);
    int serverFd, dummyFd, clientFd;
    char clientFifo[CLIENT_FIFO_NAME_LEN];

    char clientSem[CLIENT_SEM_NAME_LEN];
    char clientSem2[CLIENT_SEM_NAME_LEN];
    char clientQueue[CLIENT_SEM_NAME_LEN];
    sem_t *sema, *sema2, *sema3;
    sem_t *countQueue;
    sem_t *semaQueue;
    struct request req;

    int count;
    umask(0);

    if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST)
    {
        fprintf(stderr, "mkfifo error\n");
        exit(1);
    }

    fprintf(stdout, "Server Started PID  %d\n", getpid());
    fprintf(stdout, "waiting for clients..\n");

    logFileDescriptor = open("log.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (logFileDescriptor == -1)
    {
        fprintf(stderr, "Log File Error\n");
        return 1;
    }

    semaQueue = sem_open(CLIENT_SEM_QUEUE, O_CREAT, 0666, 0);
    sem_init(semaQueue, 0, max_clients);

    logFile(getpid(), 0, logFileDescriptor, 0);
    sema3 = sem_open(LOG_FILE_SEM, O_CREAT, 0666, 0);

    sem_post(sema3);
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
        int flag;
        if (read(serverFd, &req, sizeof(struct request)) != sizeof(struct request))
        {
            fprintf(stderr, "error reading\n");
            continue;
        }

        snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMP, (long)req.pid);
        clientFd = open(clientFifo, O_WRONLY);

        if (getpid() != req.serverPid)
        {
            flag = handle_client("error", req.pid, clientFd);
            close(clientFd);
            unlink(clientFifo);
            continue;
        }

        snprintf(clientSem, CLIENT_SEM_NAME_LEN, CLIENT_SEM_TEMP, (long)req.pid);
        sema = sem_open(clientSem, O_CREAT, 0666, 0);

        snprintf(clientSem2, CLIENT_SEM_NAME_LEN, CLIENT_SEM2_TEMP, (long)req.pid);
        sema2 = sem_open(clientSem2, O_CREAT, 0666, 0);

        flag = handle_client(req.arr, req.pid, clientFd);
        clientTable[clientNumber] = req.pid;
        clientNumber++;
        pid_t pid = fork();
        if (pid == 0)
        {

            if (flag == 1)
            {
                sem_wait(sema3);
                logFile(getpid(), clientNumber, logFileDescriptor, 1);
                sem_post(sema3);
            }
            int clientFd2 = open(clientFifo, O_RDONLY);

            struct response resp;
            while (flag == 1)
            {

                if (read(clientFd2, &resp, sizeof(struct response)) != sizeof(struct response))
                {
                    fprintf(stderr, "error reading2\n");
                    continue;
                }
                sem_post(sema);
                flag = handle_client(resp.arr, req.pid, clientFd);
                sem_wait(sema2);
            }
            sem_wait(sema3);
            logFile(getpid(), clientNumber, logFileDescriptor, 2);
            sem_post(sema3);
        }
        else if (pid > 0)
        {
            forkTable[clientNumber - 1] = pid;
        }
    }
    close(logFileDescriptor);

    unlink(SERVER_FIFO);
}
int handle_client(char *buffer, pid_t pid, int clientFd)
{
    struct response resp;
    char data[6][16];
    char delimiters[] = " ";
    int count = 0;
    char *token = strtok(buffer, delimiters);

    while (token != NULL)
    {
        strcpy(data[count++], token);
        token = strtok(NULL, delimiters);
    }

    if (strcmp(data[0], "connect") == 0)
    {
        char *help_message = "connected\0";
        strcpy(resp.arr, help_message);
        if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
        {
            fprintf(stderr, " error waiting to fifo %d\n", clientFd);
        }
        return 1;
    }
    else if (strcmp(data[0], "help") == 0)
    {
        if (count == 1)
        {
            char *help_message = "Available commands: \nhelp, list, read, write, upload, download, quit\n\0";
            strcpy(resp.arr, help_message);
        }
        else
        {
            strcpy(resp.arr, helpRequest(data[1]));
        }
        if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
        {
            fprintf(stderr, " error waiting to fifo %d\n", clientFd);
        }

        return 1;
    }

    else if (strcmp(data[0], "list") == 0)
    {
        char help_message[256];
        DIR *directory;
        struct dirent *file;

        directory = opendir(".");
        if (directory)
        {
            while ((file = readdir(directory)) != NULL)
            {
                strcat(help_message, "\n");
                strcat(help_message, file->d_name);
            }
            closedir(directory);
        }
        else
        {
            fprintf(stderr, "Error loading\n");
            return 1;
        }

        strcpy(resp.arr, help_message);

        if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
        {
            fprintf(stderr, " error waiting to fifo %d\n", clientFd);
        }
        return 1;
    }
    else if (strcmp(data[0], "readF") == 0)
    {
        if (count == 3)
        {
            int lineNumber = atoi(data[2]);
            readFile(data[1], lineNumber, resp.arr);
        }
        else
            readFile(data[1], -1, resp.arr);
        if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
        {
            fprintf(stderr, " error waiting to fifo %d\n", clientFd);
        }
        return 1;
    }
    else if (strcmp(data[0], "readF") == 0)
    {
        if (count == 3)
        {
            int lineNumber = atoi(data[2]);
            readFile(data[1], lineNumber, resp.arr);
        }
        else
            readFile(data[1], -1, resp.arr);
        if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
        {
            fprintf(stderr, " error waiting to fifo %d\n", clientFd);
        }
        return 1;
    }
    else if (strcmp(data[0], "writeT") == 0)
    {
        if (count == 4)
        {
            int lineNumber = atoi(data[2]);
            writeFile(data[1], lineNumber, data[3]);
        }
        else
        {
            writeFile(data[1], -1, data[2]);
        }
        strcpy(resp.arr, "write successful\n");
        if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
        {
            fprintf(stderr, " error waiting to fifo %d\n", clientFd);
        }
        return 1;
    }
    else if (strcmp(data[0], "quit") == 0)
    {
        char *help_message = "quit\0";
        strcpy(resp.arr, help_message);
        if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
        {
            fprintf(stderr, " error waiting to fifo %d\n", clientFd);
        }
        clientTable[clientNumber] = -1;
        return -1;
    }

    else if (strcmp(data[0], "try") == 0)
    {
        char *help_message = "try\0";
        strcpy(resp.arr, help_message);
        if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
        {
            fprintf(stderr, " error waiting to fifo %d\n", clientFd);
        }
        return -1;
    }
    else if (strcmp(data[0], "error") == 0)
    {
        char *help_message = "error\0";
        strcpy(resp.arr, help_message);
        if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
        {
            fprintf(stderr, " error waiting to fifo %d\n", clientFd);
        }
        return -1;
    }

    return 1;
}
int readFile(const char *filename, int lineNumber, char *resp)
{
    FILE *file;
    char line[256];
    int fileCount = 1;
    file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "file not opened: %s\n", filename);
        return -1;
    }

    if (lineNumber == -1)
    {
        while (fgets(line, 256, file) != NULL)
        {
            strncat(resp, line, strlen(line));
        }
    }
    else
    {
        while (fgets(line, 256, file) != NULL)
        {
            if (fileCount == lineNumber)
            {
                strncat(resp, line, strlen(line));
                break;
            }
            fileCount++;
        }
    }
    fclose(file);
    return 1;
}
int writeFile(const char *filename, int lineNumber, const char *str)
{
    int fd;
    int line = 1;

    if (lineNumber == -1)
    {
        fd = open(filename, O_RDWR | O_APPEND | O_CREAT, 0666);
    }
    else
        fd = open(filename, O_RDWR | O_CREAT, 0666);
    if (fd == -1)
    {
        fprintf(stderr, "writeFile failed\n");
        return -1;
    }
    char ch;
    while (read(fd, &ch, sizeof(ch)) > 0)
    {
        if (ch == '\n')
            line++;
        if (line == 2)
            break;
    }
    ssize_t bytesWrite = write(fd, str, strlen(str));

    if (close(fd) == -1)
    {
        return -1;
    }
    return 1;
}
char *helpRequest(char *arr)
{

    if (strcmp(arr, "readF") == 0)
    {
        return "readF <file> <line #>\ndisplay the #th line of the <file>, returns with an\nerror if <file> does not exists\n\0";
    }
    else if (strcmp(arr, "writeT") == 0)
    {
        return "writeT <file> <line #> <string>\ndisplay the #th line of the <file>, returns with an\nerror if <file> does not exists  <string> write string \n\0";
    }

    else if (strcmp(arr, "upload") == 0)
    {
        return "uploads the file from the current working directory of client to the Servers directory\n\0";
    }
    else if (strcmp(arr, "download") == 0)
    {
        return "request to receive <file> from Servers directory to client side\n\0";
    }
    else if (strcmp(arr, "quit") == 0)
    {
        return "Send write request to Server side log file and quits\n\0";
    }
    else if (strcmp(arr, "killServer") == 0)
    {
        return "Sends a kill request to the Server\n\0";
    }
    return "Invalid command please try again\n\0";
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
    if (flag == 0)
    {
        snprintf(message, sizeof(message), ">>Server Started PID %d... \n>> waiting for clients...\n", pid);
        if (write(logFileDescriptor, message, strlen(message)) == -1)
        {
            fprintf(stderr, "Error writing log file descriptor\n");

            return -1;
        }
    }

    else if (flag == 1)
    {
        snprintf(message, sizeof(message), ">>Client PID %d connected as  client%d\n", pid, clientNumber);
        if (write(logFileDescriptor, message, strlen(message)) == -1)
        {
            fprintf(stderr, "Error writing log file descriptor\n");

            return -1;
        }
    }

    else if (flag == 2)
    {

        snprintf(message, sizeof(message), ">>client %d disconnected.. \n", clientNumber);
        if (write(logFileDescriptor, message, strlen(message)) == -1)
        {
            fprintf(stderr, "Error writing log file descriptor\n");

            return -1;
        }
    }
    else if (flag == 3)
    {

        snprintf(message, sizeof(message), ">>Connection request PID %d...  Que FULL", pid);
        if (write(logFileDescriptor, message, strlen(message)) == -1)
        {
            fprintf(stderr, "Error writing log file descriptor\n");

            return -1;
        }
    }

    fl.l_type = F_UNLCK;
    if (fcntl(logFileDescriptor, F_SETLK, &fl) == -1)
    {
        fprintf(stderr, "File open lock error\n");
        return -1;
    }

    return 0;
}
