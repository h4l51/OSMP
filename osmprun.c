//
// Created by Lukas Böing on 08.04.21. for Praktikum 1
//
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
extern int errno;

#include "OSMPLib.h"
#include "osmprun.h"

char *env_init[] = { "USER=unknown", "PATH=\\tmp", NULL};

void printLastError()
{
        printf("\nError in file: %s - line %d - %s", __FILE__, __LINE__, strerror(errno));
        fflush(stdout);
}

void printInLoop(){
    pid_t pid = fork();
    if(pid == OSMP_ERROR){ //Fehlerbehandlung

        printLastError();
        exit(OSMP_ERROR);
    }
    for(int i = 0; i<10; i++){
        printf("Ausgabe %d von Prozess mit PID: %d \n", i, pid);
        sleep(1);
    }

    if(pid == OSMP_SUCCESS){
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    printInLoop();
    pid_t c_pid1;
    for(int i = 0; i < 6; i++)
    {
        c_pid1 = fork();
        if(c_pid1 == OSMP_ERROR) //fork failed
        {
            printLastError();
            return -1;
        }
        else if(c_pid1 == OSMP_SUCCESS) //child process
        {
           switch (i)
           {
            case 0:
            {
                execl(  "./echoall",
                        "./echoall",
                        "argument1",
                        "execl",
                        NULL    
                );
                break;
            }
            case 1:
            {
                execle( "./echoall",
                        "./echoall",
                        "argument1",
                        "execle",
                        NULL,
                        env_init
                );
                break;
            }
            case 2:
            {
                execlp( "./echoall",
                        "./echoall",
                        "argument1",
                        "execlp",
                        NULL
                );
                break;
            }
            case 3:
            {
                char* vars[] = {"./echoall", "argument1", "execv", NULL};
                execv(  "./echoall",
                         vars
                );
                break;
            }
            case 4:
            {
                char* vars[] = {"./echoall", "argument1", "execve", NULL};
                execve( "./echoall",
                        vars,
                        env_init
                );
                break;
            }
            case 5:
            {
                char* vars[] = {"./echoall", "argument1", "execvp", NULL};
                execvp( "./echoall",
                        vars
                );
                break;
            }
            default:
                break;
           }
            printLastError();
            return -1;
        }
        else //parent process
        {
            int wstatus;
            if(waitpid(c_pid1, &wstatus, 0) == OSMP_ERROR)
                printLastError();
            
            printf("\nchild process %d ended return value: %d\n", c_pid1, wstatus);
            fflush(stdout);
        }
    }
    printf("\n");
    return 0;
}