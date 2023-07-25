#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

int main(int argc, char **argv)
{
    int flag = 0;
    if (argc < 3 || argc > 4)
    {
        errno=EINVAL;
        perror("Please Enter Correct Entry");
        return -1;
    }

    if (argc == 3)
    {
        flag = 1;
    }
    else if (argc == 4)
    {
        if (strcmp(argv[3], "x") == 0 || strcmp(argv[3], "X") == 0)
        {
            flag = 2;
        }
        else
        {
            errno=EINVAL;
            perror("Invalid command line argument");
            return -1;
        }
    }
    else
    {
        errno=EINVAL;
        perror("Invalid command line argument");
        return -1;
    }

    char *fileName = argv[1];
    int numBytes = atoi(argv[2]);

    int fd;
    if (flag == 1)
    {
        fd = open(fileName, (O_CREAT | O_WRONLY | O_APPEND), 0666);
        for (int i = 0; i < numBytes; i++)
        {
            
            if (write(fd,"a", 1) != 1)
            {
                perror("write");
                return -1;
            }
        }
    }
    else
    {
        fd = open(fileName, (O_CREAT | O_WRONLY), 0666);
        for (int i = 0; i < numBytes; i++)
        {
            if (lseek(fd, 0, SEEK_END) < 0)
            {
                perror("lseek");
                close(fd);
                return -1;
            }          

            if (write(fd, "a", 1) != 1)
            {
                perror("write");
                close(fd);
                return -1;
            }
        }
    }

    if (close(fd) < 0)
    {
        perror("close");
        return 1;
    }

    return 0;
}