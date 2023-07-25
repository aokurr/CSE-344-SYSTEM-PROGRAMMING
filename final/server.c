#include "commonLib.h"
#include "queue.h"
#define MAX_CLIENTS 250
int count = 0;
int N = 1024;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
Queue *queue;
char directory[128];
void *consumer(void *arg);
void handler(int signal)
{
    if (signal == SIGINT)
    {
        write(1, "\nSIGINT Cached.\n", 17);
        destroyQueue(queue);
        write(1, "Signal handled\n", 16);
        exit(1);
    }
}
int main(int argc, char *argv[])
{
    int opt = 1;
    if (argc != 4)
    {
        fprintf(stderr, "Usage error : argc!=4 \n Should be -> ./BibakBOXServer [directory] [threadPoolSize] [portnumber]\n");
        return 1;
    }

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

    queue = createQueue(queue);
    int poolSize = atoi(argv[2]);
    int PORT = atoi(argv[3]);
    strcpy(directory, argv[1]);
    pthread_t *threads = malloc(poolSize * sizeof(pthread_t));
    for (int i = 0; i < poolSize; i++)
    {
        pthread_create(&threads[i], NULL, consumer, NULL);
    }

    int server_sock, client_sock, c;
    struct sockaddr_in server, client;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1)
    {
        perror("Could not create socket");
        return 1;
    }
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Bind failed");
        return 1;
    }

    if (listen(server_sock, 2) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Server listening on port %d\n", PORT);

    c = sizeof(struct sockaddr_in);

    while ((client_sock = accept(server_sock, (struct sockaddr *)&client, (socklen_t *)&c)))
    {

        fprintf(stdout, "Connection accepted.\n");
        pthread_mutex_lock(&mutex);
        while (count == N)
        {
            pthread_cond_wait(&empty, &mutex);
        }
        enqueue(queue, client_sock);
        count++;

        pthread_cond_broadcast(&full);
        pthread_mutex_unlock(&mutex);
    }

    for (int i = 0; i < poolSize; i++)
    {
        pthread_join(threads[i], NULL);
    }

    if (client_sock < 0)
    {
        perror("Accept failed");
        return 1;
    }

    close(server_sock);
    return 0;
}
void *consumer(void *arg)
{

    while (1)
    {

        pthread_mutex_lock(&mutex);

        while (count == 0)
        {
            pthread_cond_wait(&full, &mutex);
        }
        int sock = dequeue(queue);

        count--;

        pthread_cond_broadcast(&empty);
        pthread_mutex_unlock(&mutex);
        struct fileInfo file2[256];
        struct fileInfo files[256];
        struct fileInfo removedFiles[256];
        int countServerFile = 0;
        int countFile2 = 0;
        int countRemovedFiles = 0;
        int flag = 0;
        while (1)
        {
            if (flag == 1)
            {
                countFile2 = 0;
                listDir(directory, 0, file2, &countFile2, strlen(directory));
                countRemovedFiles = SaveDifferentElements(files, file2, removedFiles, countServerFile, countFile2);
                
            }

            struct fileInfo receiveFiles[256];
            struct fileInfo sendFilesClient[256];
            struct fileInfo differencesFilesClient[256];
            countServerFile = 0;
            listDir(directory, 0, files, &countServerFile, strlen(directory));
            
            sendFileInfo(sock, files, countServerFile,removedFiles,countRemovedFiles);
            int countRecvFile = receiveFileInfo(sock, receiveFiles,directory);
            if (countRecvFile == -1)
            {
                write(1, "\nAny client logged out...\n", 27);
                break;
            }
            int countSendFilesCl = SaveDifferentElements(files, receiveFiles, sendFilesClient, countServerFile, countRecvFile);
            int countDifFilesClient = SaveDifferentElements(receiveFiles, files, differencesFilesClient, countRecvFile, countServerFile);

            flag = 1;
            sendFile(sendFilesClient, countSendFilesCl, sock, directory);
            recvFile(differencesFilesClient, countDifFilesClient, sock, directory);
            //sleep(3);
        }
    }

    return NULL;
}