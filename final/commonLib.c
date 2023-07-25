#include "commonLib.h"
#include <sys/stat.h>
void sendFileInfo(int sock, struct fileInfo *files, int countServerFile, struct fileInfo *removedFiles, int countRemovedFiles)
{
    struct fileInfo tmp;

    strcpy(tmp.name, "FILE_NAMES\0");
    tmp.modification_time = 0;
    tmp.st_size = 0;
    send(sock, &tmp, sizeof(struct fileInfo), 0);
    for (int i = 0; i < countServerFile; i++)
    {
        files[i].isDeleted = 0;
        send(sock, &files[i], sizeof(struct fileInfo), 0);
    }
    for (int i = 0; i < countRemovedFiles; i++)
    {
        removedFiles[i].isDeleted = 1;
        send(sock, &removedFiles[i], sizeof(struct fileInfo), 0);
    }
    strcpy(tmp.name, "END\0");
    tmp.modification_time = 0;
    tmp.st_size = 0;
    send(sock, &tmp, sizeof(struct fileInfo), 0);
}

int receiveFileInfo(int sock, struct fileInfo *files, char *directory)
{
    struct fileInfo fileFlag;
    int i = 0;
    while (1)
    {
        recv(sock, &fileFlag, sizeof(struct fileInfo), 0);

        if (strcmp(fileFlag.name, "FILE_NAMES") == 0)
            continue;
        ;
        if (strcmp(fileFlag.name, "END") == 0)
            return i;
        if (strcmp(fileFlag.name, "FINISH") == 0)
        {
            return -1;
        }
        if (fileFlag.isDeleted == 1)
        {
            char path[256];
            snprintf(path, sizeof(path), "%s%s", directory, fileFlag.path);

            removeFileAndDirectory(path);
            continue;
        }
        strcpy(files[i].name, fileFlag.name);
        strcpy(files[i].path, fileFlag.path);
        files[i].modification_time = fileFlag.modification_time;
        files[i].st_size = fileFlag.st_size;
        files[i].is_directory = fileFlag.is_directory;
        files[i].mode = fileFlag.mode;

        i++;
    }
}
int SaveDifferentElements(struct fileInfo *files, struct fileInfo *receiveFiles, struct fileInfo *lastFiles, int countMyFile, int countRecvFile)
{
    int i, j, k = 0;
    int flag;

    for (i = 0; i < countMyFile; i++)
    {
        flag = 0;
        for (j = 0; j < countRecvFile; j++)
        {
            if (strcmp(files[i].path, receiveFiles[j].path) == 0 && files[i].st_size == receiveFiles[j].st_size)
            {
                flag = 1;
                break;
            }
        }

        if (!flag)
        {
            if (strcmp(files[i].name, receiveFiles[j].name) == 0 && files[i].st_size < receiveFiles[j].st_size)
            {
                continue;
            }
            strcpy(lastFiles[k].name, files[i].name);
            strcpy(lastFiles[k].path, files[i].path);
            lastFiles[k].modification_time = files[i].modification_time;
            lastFiles[k].st_size = files[i].st_size;
            lastFiles[k].is_directory = files[i].is_directory;
            lastFiles[k].mode = files[i].mode;

            k++;
        }
    }
    return k;
}
void writeFile(char const *fileName, int new_sock, size_t fileSize)
{
    int n = 0;
    int fp;
    char ch;

    fp = open(fileName, O_WRONLY | O_CREAT, 0644);
    if (fp < 0)
    {
        fprintf(stdout, "filename = %s\n", fileName);
        perror("[-]Error in opening file1");

        exit(1);
    }
    while (1)
    {
        n++;
        if (fileSize < n)
        {
            break;
        }
        if (recv(new_sock, &ch, 1, 0) <= 0)
        {
            break;
        }
        write(fp, &ch, 1);
    }
    close(fp);
}
void recvFile(struct fileInfo files[256], int count, int sockfd, char *directory)
{
    for (int i = 0; i < count; i++)
    {
        char path[1024];
        snprintf(path, sizeof(path), "%s%s", directory, files[i].path);
        if (files[i].is_directory == 1)
        {
            mkdir(path, files[i].mode);
            continue;
        }

        writeFile(path, sockfd, files[i].st_size);
    }
}
void readFile(char *fileName, int sockfd)
{
    off_t n;
    char data;
    int fp = open(fileName, O_RDONLY, 0644);
    while ((n = read(fp, &data, 1)) > 0)
    {
        if (send(sockfd, &data, 1, 0) == -1)
        {
            perror("[-]Error in sending file.1");
            exit(1);
        }
    }
    close(fp);
}

void sendFile(struct fileInfo files[256], int count, int sockfd, char *directory)
{
    for (int i = 0; i < count; i++)
    {
        if (files[i].is_directory == 1)
            continue;

        char path[1024];
        snprintf(path, sizeof(path), "%s%s", directory, files[i].path);
        readFile(path, sockfd);
    }
}

void addFileData(struct fileInfo *files, int index, const char *name, const char *path, mode_t mode, off_t size, int is_directory, int directorySize)
{
    if (index >= 256)
    {
        fprintf(stderr, "Maximum array length\n");
        return;
    }

    struct fileInfo data;
    int k = 0;
    for (int i = directorySize; i < strlen(path); i++)
    {
        data.path[k++] = path[i];
    }
    data.path[k++] = '\0';
    strncpy(data.name, name, sizeof(data.name));
    data.mode = mode;
    data.st_size = size;
    data.is_directory = is_directory;

    files[index] = data;
}

void listDir(const char *name, int indent, struct fileInfo *files, int *count, int directorySize)
{
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;

    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL)
    {
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);

        if (entry->d_type == DT_DIR)
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            if (stat(path, &fileStat) == 0)
            {
                addFileData(files, *count, entry->d_name, path, fileStat.st_mode, -1, 1, directorySize);
                (*count)++;
            }
            listDir(path, indent + 2, files, count, directorySize);
        }
        else
        {
            if (stat(path, &fileStat) == 0)
            {
                if (strcmp(entry->d_name, "logAhmet.txt") == 0)
                    continue;
                addFileData(files, *count, entry->d_name, path, fileStat.st_mode, fileStat.st_size, 0, directorySize);
                (*count)++;
            }
        }
    }
    closedir(dir);
}
int removeFileAndDirectory(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);

    if (S_ISREG(path_stat.st_mode))
    {
        return remove(path);
    }
    else if (S_ISDIR(path_stat.st_mode))
    {
        return removeDirectory(path);
    }
    else
    {
        return -1;
    }
}
int removeDirectory(const char *path)
{
    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    int r = -1;

    if (d)
    {
        struct dirent *p;

        r = 0;

        while (!r && (p = readdir(d)))
        {
            int r2 = -1;
            char *buf;
            size_t len;

            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
            {
                continue;
            }

            len = path_len + strlen(p->d_name) + 2;
            buf = malloc(len);

            if (buf)
            {
                struct stat statbuf;

                snprintf(buf, len, "%s/%s", path, p->d_name);

                if (!stat(buf, &statbuf))
                {
                    if (S_ISDIR(statbuf.st_mode))
                    {
                        r2 = removeDirectory(buf);
                    }
                    else
                    {
                        r2 = remove(buf);
                    }
                }

                free(buf);
            }

            r = r2;
        }

        closedir(d);
    }

    if (!r)
    {
        r = remove(path);
    }

    return r;
}