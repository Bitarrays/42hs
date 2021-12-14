#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int execpipe(char ***args, int numpipes)
{
    int i;
    int commandc = 0;
    pid_t pid;
    int *pipefds = (int *)malloc((2 * numpipes) * sizeof(int));
    for (i = 0; i < numpipes; i++)
    {
        if (pipe(pipefds + i * 2) < 0)
        {
            perror("pipe creation failed");
            return 3;
        }
    }
    int args_index = 0;
    do
    {
        pid = fork();
        if (pid == 0)
        {
            if (commandc != 0 && dup2(pipefds[(commandc - 1) * 2], 0) < 0)
            {
                perror("child couldnt get input");
                exit(1);
            }
            if (commandc != numpipes && dup2(pipefds[commandc * 2 + 1], 1) < 0)
            {
                perror("child couldnt output");
                exit(1);
            }
            for (i = 0; i < 2 * numpipes; i++)
                close(pipefds[i]);
            if (!strcmp(args[args_index][0], "echo"))
            {
                printf("a\nb\nc\nd\n");
                return 0;
            }
            else
            {
                execvp(args[args_index][0], args[args_index]);
                perror("exec failed");
                exit(1);
            }
        }
        else if (pid < 0)
        {
            perror("fork() failed");
            return 3;
        }
        
        commandc++;
        args_index++;
    } while (commandc < numpipes + 1);

    for (i = 0; i < 2 * numpipes; i++)
        close(pipefds[i]);
    free(pipefds);
    return 1;
}

int main(void)
{
    char ***args = calloc(2, sizeof(char **));
    args[0] = calloc(3, sizeof(char *));
    args[1] = calloc(4, sizeof(char *));
    args[0][0] = "echo";
    args[0][1] = "-l";
    args[0][2] = NULL;
    args[1][0] = "tail";
    args[1][1] = "-n";
    args[1][2] = "1";
    args[1][3] = NULL;
    execpipe(args, 1);
    fflush(stdout);
}
