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

#include "OSMP.h"
#include "osmprun.h"

//for posix shared memory
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */


char *env_init[] = { "USER=unknown", "PATH=\\tmp", NULL};

void printLastError() /// Prints out the last error
{
        printf("\nError in file: %s - line %d - %s", __FILE__, __LINE__, strerror(errno));
        fflush(stdout);
}

void printInLoop()  /// Prints out the PID and an Int from 1-10
{
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

void execTest() /// goes through all the various types of exec()-methods and tries them out
{
    pid_t c_pid1;
    for(int i = 0; i < 6; i++)
    {
        c_pid1 = fork();
        if(c_pid1 == OSMP_ERROR) //fork failed
        {
            printLastError();
            exit(OSMP_ERROR);
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
            exit(OSMP_ERROR);
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

}

int main(int argc, char *argv[])/// Main method of osmprun.c,
/// \param argc
/// \param argv
/// \return
{
    //char *username = getenv("USER");
    char szProcessID[32];
    pid_t processID = getpid();

    if(argc < 3)
    {
        printf("Not enough arguments in function call.\n");
        exit(OSMP_ERROR);
    }


    OSMP_INT nProcessNum = 0;
    OSMP_INT returnVal = sscanf(argv[1], "%d", &nProcessNum);
    if(returnVal == EOF || returnVal <= 0 || nProcessNum <= 0)
    {
        printf("Argument 1 invalid, expected number of processes.\n");
        exit(OSMP_ERROR);
    }

    //init shared memory
    if(sprintf(szProcessID, "%d", processID) <= 0)
    {
        exit(OSMP_ERROR);
    }
    char* sharedMemoryName = calloc(strlen(OSMP_SHMEM_NAME) + strlen(szProcessID) + 1, (sizeof(char)));
    if(sprintf(sharedMemoryName, "%s%s", OSMP_SHMEM_NAME, szProcessID) <= 0)
    {
        free(sharedMemoryName);
        exit(OSMP_ERROR);
    }

    OSMP_INT shmFileDescriptor = shm_open(sharedMemoryName, O_RDWR | O_CREAT,  ACCESSPERMS);
    if(shmFileDescriptor == OSMP_ERROR)
    {
        printf("Creating shared memory failed.\n");
        printLastError();
        shm_unlink(sharedMemoryName);
        free(sharedMemoryName);
        exit(OSMP_ERROR);
    }
    else
        printf("shared memory created, shmName: %s - fd: %d\n", sharedMemoryName, shmFileDescriptor);


    struct stat testStat;

    fstat(shmFileDescriptor, &testStat);
    printf("shm size: %ld\n", testStat.st_size);

    printf("changing shm size...\n");
    if(ftruncate(shmFileDescriptor, 64) == OSMP_ERROR)
    {
        printf("failed to set shm size.");
        printLastError();
        shm_unlink(sharedMemoryName);
        free(sharedMemoryName);
        exit(OSMP_ERROR);
    }

    fstat(shmFileDescriptor, &testStat);
    printf("shm size: %ld\n", testStat.st_size);

    //sleep(10);
    shm_unlink(sharedMemoryName);
    printf("unlinked from shared memory\n");
    free(sharedMemoryName);
    printf("\n");
    return 0;
}

