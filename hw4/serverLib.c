#include "serverLib.h"

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
        int value;
        sem_t *clientCountSem;
        clientCountSem = sem_open(CLIENT_MAX, 0);
        sem_getvalue(clientCountSem, &value);
        char *help_message;
        if (value == 0)
        {
            help_message = "try\0";
        }
        else
        {
            int logFileDescriptor = open("log.txt", O_APPEND, 0644);
            if (logFileDescriptor == -1)
            {
                fprintf(stderr, "Log File Error\n");
                return 1;
            }
            help_message = "connected\0";
        }
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
        sem_t *clientCountSem;
        
        char *help_message = "quit\0";
        strcpy(resp.arr, help_message);
        
        if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
        {
            fprintf(stderr, " error waiting to fifo %d\n", clientFd);
        }
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
        snprintf(message, sizeof(message), ">>Client PID %d connected \n", pid);
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