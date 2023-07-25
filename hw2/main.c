#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#define BUFF_SIZE 1024
int isRedirection(char *arr)
{
    for (int i = 0; i < strlen(arr); i++)
    {
        if (arr[i] == '<')
            return 0;
        if (arr[i] == '>')
            return 1;
    }
    return 2;
}
void redirection(char arr[20], int flag, int pip[2])
{

    char *pch;
    pch = strtok(arr, "<>");
    char command[20][20];
    int i = 0;
    while (pch != NULL)
    {
        strcpy(command[i++], pch);
        pch = strtok(NULL, "<>");
    }

    int fd = open(command[flag], O_CREAT | O_WRONLY, 0644);

    if (close(pip[1]) == -1)
    {
        _exit(-1);
    }

    if (dup2(pip[0], STDIN_FILENO) == -1)
    {
        _exit(-1);
    }

    if (dup2(fd, STDOUT_FILENO) == -1)
    {
        _exit(-1);
    }
    close(fd);
    execl("/bin/sh", "sh", "-c", command[!(flag)], (char *)NULL);
}
void handler(int signal_number)
{
    write(STDOUT_FILENO, "signal handeled", 15);
}
int main(int argc, char **argv)
{

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &handler;
    sigaction(SIGINT, &sa, NULL);

    while (1)
    {
        char buff[BUFF_SIZE];
        int pfd[20][2];
        char *pch;
        char buf[120];
        int pipe_count = -2;
        char command[20][20];
        int cCount = 0;
        int size = read(STDIN_FILENO, buff, BUFF_SIZE);

        buff[size - 1] = '\0';
        pch = strtok(buff, "|");
        while (pch != NULL)
        {
            strcpy(command[cCount++], pch);
            pch = strtok(NULL, "|");
        }
        if (strcmp(command[0], ":q") == 0)
        {
            return -1;
        }
        for (int i = 0; i < cCount - 1; i++)
        {
            if (pipe(pfd[i]) == -1)
                return -1;
        }

        for (int i = 0; i < cCount; i++)
        {

            pipe_count++;
            int childPid;
            int status;

            switch (fork())
            {
            case -1:
                _exit(-1);
            case 0:
                if (cCount == 1)
                {
                    execl("/bin/sh", "sh", "-c", command[i], (char *)NULL);
                }
                else if (i == 0)
                {
                    if (close(pfd[0][0]) == -1)
                    {
                        _exit(-1);
                    }

                    if (pfd[0][1] != STDOUT_FILENO)
                    {
                        if (dup2(pfd[0][1], STDOUT_FILENO) == -1)
                        {
                            _exit(-1);
                        }
                        if (close(pfd[0][1]) == -1)
                        {
                            _exit(-1);
                        }
                    }

                    execl("/bin/sh", "sh", "-c", command[i], (char *)NULL);
                    _exit(-1);
                }
                else

                {
                    int flag;
                    if ((flag = isRedirection(command[i])) != 2)
                    {
                        redirection(command[i], flag, pfd[pipe_count]);
                    }

                    if (close(pfd[pipe_count][1]) == -1)
                    {
                        _exit(-1);
                    }

                    if (pfd[pipe_count][0] != STDIN_FILENO)
                    {
                        if (dup2(pfd[pipe_count][0], STDIN_FILENO) == -1)
                        {
                            _exit(-1);
                        }
                        if (close(pfd[pipe_count][0]) == -1)
                        {
                            _exit(-1);
                        }
                    }

                    if (i != cCount - 1)

                    {

                        if (close(pfd[pipe_count + 1][0]) == -1)
                        {
                            _exit(-1);
                        }

                        if (pfd[pipe_count + 1][1] != STDOUT_FILENO)
                        {
                            if (dup2(pfd[pipe_count + 1][1], STDOUT_FILENO) == -1)
                            {
                                _exit(-1);
                            }

                            if (close(pfd[pipe_count + 1][1]) == -1)
                            {
                                _exit(-1);
                            }
                        }
                    }

                    execl("/bin/sh", "sh", "-c", command[i], (char *)NULL);
                    _exit(-1);
                }
            default:
                break;
            }
            if (i > 0)
            {
                close(pfd[pipe_count][0]);
                close(pfd[pipe_count][1]);
            }
        }
    }
    return 0;
}