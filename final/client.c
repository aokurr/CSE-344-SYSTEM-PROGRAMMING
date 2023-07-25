// client.c
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include "commonLib.h"
#include <time.h>
#define SERVER_IP "127.0.0.1"
int sock = 0;
char directory[128];
int logFd;

void logFile(char *name, char *path, int flag, int isFolder);
void logging(struct fileInfo *files, int count, int flag);
void handler(int signal)
{
    if (signal == SIGINT)
    {
        write(1, "\nSIGINT Cached.\n", 17);
        char buffer[1024];
        strcpy(buffer, "FINISH");
        send(sock, buffer, sizeof(buffer), 0);
    }
    else if (signal == SIGPIPE) {
        write(1, "\nSIGPIPE Cached.\n", 18);
        close(sock);
    }
    
    write(1, "Signal handled\n", 16);
    logFile(NULL, NULL, 3, 0);
    close(logFd);
    exit(1);
}
int main(int argc, char *argv[])
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &handler;
    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction");
        return 1;
    }

    if (sigaction(SIGPIPE, &sa, NULL) == -1)
    {
        perror("sigaction");
        return 1;
    }
    struct sockaddr_in serv_addr;
    if (argc != 3)
    {
        fprintf(stderr, "Usage error : argc!=4 \n Should be -> ./BibakBOXServer [directory] [portnumber]\n");
        return 1;
    }
    int PORT = atoi(argv[2]);
    strcpy(directory, argv[1]);
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address or Address not supported");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection error");
        return -1;
    }
    char logPath[256];
    snprintf(logPath, sizeof(logPath), "%s/logAhmet.txt", directory);

    logFd = open(logPath, O_WRONLY | O_CREAT | O_APPEND, 0644);

    logFile(NULL, NULL, 4, 0);
    int i = 0;
    while (1)
    {

        struct fileInfo files[256];
        struct fileInfo receiveFiles[256];

        struct fileInfo sendFilesServer[256];
        struct fileInfo differencesFilesServer[256];

        int countClntFile = 0;
        
        int countRecvFile = receiveFileInfo(sock, receiveFiles,directory);
        listDir(directory, 0, files, &countClntFile, strlen(directory));
        sendFileInfo(sock, files, countClntFile,NULL,0);

        int countSendFilesServ = SaveDifferentElements(files, receiveFiles, sendFilesServer, countClntFile, countRecvFile);

        int countDifFilesServ = SaveDifferentElements(receiveFiles, files, differencesFilesServer, countRecvFile, countClntFile);

        recvFile(differencesFilesServer, countDifFilesServ, sock, directory);
        if (countDifFilesServ != 0)
            logging(differencesFilesServer, countDifFilesServ, 2);

        sendFile(sendFilesServer, countSendFilesServ, sock, directory);
        if (countSendFilesServ != 0)
            logging(sendFilesServer, countSendFilesServ, 1);
    }

    return 0;
}
void logFile(char *name, char *path, int flag, int isFolder)
{
    struct flock fl;
    char buff[256];
    if (logFd == -1)
    {
        perror("open");
        return;
    }
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);

    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;

    if (fcntl(logFd, F_SETLKW, &fl) == -1)
    {
        perror("fcntl");
        close(logFd);
        return;
    }

    if (flag == 1)
    {

        if (isFolder == 0)
            snprintf(buff, sizeof(buff), "client sent %s file to server | path-> %s | time-> %s\n", name, path, buffer);
        else
            snprintf(buff, sizeof(buff), "client sent %s folder to server | path-> %s | time-> %s\n", name, path, buffer);
    }

    if (flag == 2)
    {
        if (isFolder == 0)
            snprintf(buff, sizeof(buff), "client receive %s file to server | save path-> %s | time-> %s\n", name, path, buffer);
        else
            snprintf(buff, sizeof(buff), "client receive %s file to server | save path-> %s | time-> %s\n", name, path, buffer);
    }
    if (flag == 3)
    {
        snprintf(buff, sizeof(buff), "client logged out. time -> %s\n", buffer);
    }
    if (flag == 4)
    {
        snprintf(buff, sizeof(buff), "client connected. time -> %s\n", buffer);
    }

    if (write(logFd, buff, strlen(buff)) == -1)
    {
        perror("write");
        close(logFd);
        return;
    }

    fl.l_type = F_UNLCK;
    if (fcntl(logFd, F_SETLKW, &fl) == -1)
    {
        perror("fcntl");
        close(logFd);
        return;
    }
}
void logging(struct fileInfo *files, int count, int flag)
{
    for (int i = 0; i < count; i++)
    {
        logFile(files[i].name, files[i].path, flag, files[i].is_directory);
    }
}