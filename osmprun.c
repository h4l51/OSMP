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

void printLastError(char* file, int line) /// Prints out the last error
{
    printf("Error in file: %s - line %d - %s\n", file, line, strerror(errno));
    fflush(stdout);
}

void printInLoop()  /// Prints out the PID and an Int from 1-10
{
    pid_t pid = fork();
    if(pid == OSMP_ERROR){ //Fehlerbehandlung

        printLastError(__FILE__, __LINE__);
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
            printLastError(__FILE__, __LINE__);
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
            printLastError(__FILE__, __LINE__);
            exit(OSMP_ERROR);
        }
        else //parent process
        {
            int wstatus;
            if(waitpid(c_pid1, &wstatus, 0) == OSMP_ERROR)
                printLastError(__FILE__, __LINE__);

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
    off_t shmSize = 0;
    struct stat shmStat;
    OSMP_shm_info* mappingPtr;
    int nProcessNum, returnVal, shmFileDescriptor, wstatus;

    //char *username = getenv("USER");
    pid_t processID = getpid();

    if(argc < 3)
    {
        printf("Not enough arguments in function call.\n");
        exit(OSMP_ERROR);
    }


    returnVal = sscanf(argv[1], "%d", &nProcessNum);
    if(returnVal == EOF || returnVal <= 0 || nProcessNum <= 0)
    {
        printf("Argument 1 invalid, expected number of processes.\n");
        exit(OSMP_ERROR);
    }
    if(nProcessNum <= 0 || nProcessNum > 16)
    {
        printf("Invalid number of processes.\n");
        exit(OSMP_ERROR);
    }

    //init shared memory
    char* sharedMemoryName = OSMP_GetShmName(processID);

    shmFileDescriptor = shm_open(sharedMemoryName, O_RDWR | O_CREAT,  ACCESSPERMS);
    if(shmFileDescriptor == OSMP_ERROR)
    {
        printf("Creating shared memory failed.\n");
        printLastError(__FILE__, __LINE__);
        shm_unlink(sharedMemoryName);
        free(sharedMemoryName);
        exit(OSMP_ERROR);
    }
    else
        printf("shared memory created, shmName: %s - fd: %d\n", sharedMemoryName, shmFileDescriptor);

    shmSize = (off_t)(sizeof(OSMP_shm_info) + sizeof(OSMP_Proc) * (size_t)(nProcessNum - 1));

    if(ftruncate(shmFileDescriptor, shmSize) == OSMP_ERROR)
    {
        printLastError(__FILE__, __LINE__);
        shm_unlink(sharedMemoryName);
        free(sharedMemoryName);
        exit(OSMP_ERROR);
    }

    fstat(shmFileDescriptor, &shmStat);
    printf("shm size: %ld byte\n", shmStat.st_size);

    mappingPtr = (OSMP_shm_info*)mmap(NULL, (size_t)shmStat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmFileDescriptor, 0);
    if(mappingPtr == MAP_FAILED)
    {
        printf("failed to map memory.");
        printLastError(__FILE__, __LINE__);
        shm_unlink(sharedMemoryName);
        free(sharedMemoryName);
        exit(OSMP_ERROR);
    }

    //info block at shm start
    mappingPtr->nProcessCount = nProcessNum;

    pid_t c_pid;
    for(int i = 0; i < nProcessNum; i++)
    {
        c_pid = fork();
        if (c_pid == OSMP_ERROR) //fork failed
        {
            printLastError(__FILE__, __LINE__);
            continue;
        }
        else if (c_pid == OSMP_SUCCESS) //child process
        {
            execv(  argv[2],
                    &argv[2]
            );
            printLastError(__FILE__, __LINE__);
            exit(OSMP_ERROR);
        }
        else //parent process
        {
            mappingPtr->proc[i].pid = c_pid;
            //starterInfoStruct->nProcessRank[i][0] = c_pid[i];
            //starterInfoStruct->nProcessRank[i][1] = i;
            printf("child process %d started: %d\n", i, c_pid);
        }
    }

    for(int i = 0; i < nProcessNum; i++)
    {
        if(waitpid(mappingPtr->proc[i].pid, &wstatus, 0) == OSMP_ERROR)
            printLastError(__FILE__, __LINE__);
        else
        {
            printf("child process %d finished - pid: %d\n", i, mappingPtr->proc[i].pid);
        }
    }

    //sleep(10);
    returnVal = shm_unlink(sharedMemoryName);
    if(returnVal == OSMP_ERROR)
    {
        printf("failed to remove shared memory");
    }
    else
        printf("removed shared memory - return: %d\n", returnVal);

    free(sharedMemoryName);
    exit(OSMP_SUCCESS);
}

