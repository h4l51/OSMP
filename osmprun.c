//
// Created by Lukas Böing on 08.04.21. for Praktikum 1
//

#include <stdio.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "OSMPLib.h"

char *env_init[] = { "USER=unknown", "PATH=\\tmp", NULL};

int main(int argc, char *argv[])
{
    pid_t c_pid1;
    c_pid1 = fork();

    if(c_pid1 == OSMP_ERROR) //fork failed
    {
        printf("error: fork failed\n");
        return -1;
    }
    else if(c_pid1 == OSMP_SUCCESS) //child process
    {
        execle( "./echoall",
                "echoall",
                "argument1",
                "argument2",
                NULL,
                env_init
        );
        pid_t pid_failed = getpid();
        printf("execle failed for process %d ", pid_failed);
        return -1;
    }
    else //parent process
    {
        int status;
        waitpid(c_pid1, &status, 0);
        printf("parent process; wait for process %d return: %d\n", c_pid1, status);
    }
    return 0;
}