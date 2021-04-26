//
// Created by Felix Halstrup on 08.04.21.
//

#include <stdio.h>
#include "OSMP.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


//for posix shared memory
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */


char* sharedMemoryName;
int OSMP_ShmFileDescriptor;
void* OSMP_ShmPtr;

int OSMP_Init(int *argc, char ***argv)
{
    struct stat shmStat;
    int returnVal;

    sharedMemoryName = OSMP_GetShmName(getppid());

    OSMP_ShmFileDescriptor = shm_open(sharedMemoryName, O_RDWR, ACCESSPERMS);
    if(OSMP_ShmFileDescriptor == OSMP_ERROR)
    {
        return OSMP_ERROR;
    }
    returnVal = fstat(OSMP_ShmFileDescriptor, &shmStat);
    if(returnVal == OSMP_ERROR)
    {
        return OSMP_ERROR;
    }

    OSMP_ShmPtr = mmap(NULL, (size_t)shmStat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, OSMP_ShmFileDescriptor, 0);
    if(OSMP_ShmPtr == MAP_FAILED)
    {
        return OSMP_ERROR;
    }

    return OSMP_SUCCESS;
}

int OSMP_Size(int *size)
{
    return OSMP_SUCCESS;
}
int OSMP_Rank(int *rank)
{
    return OSMP_SUCCESS;
}
int OSMP_Send(const void *buf, int count, OSMP_Datatype datatype, int dest)
{
    switch(datatype)
    {
        case OSMP_SHORT: // short int
        {
            break;
        }
        case OSMP_INT: // int
        {
            break;
        }
        case OSMP_LONG: // long int
        {
            break;
        }
        case OSMP_UNSIGNED_CHAR: // unsinged char
        {
            break;
        }
        case OSMP_UNSIGNED: // unsigned int
        {
            break;
        }
        case OSMP_UNSIGNED_LONG: // unsigned long int
        {
            break;
        }
        case OSMP_FLOAT: // float
        {
            break;
        }
        case OSMP_DOUBLE: // double
        {
            break;
        }
        case OSMP_BYTE: // char
        {
            break;
        }
        default:
            break;
    }
    return OSMP_SUCCESS;
}
int OSMP_Recv(void *buf, int count, OSMP_Datatype datatype, int *source, int *len)
{
    switch(datatype)
    {
        case OSMP_SHORT: // short int
        {
            break;
        }
        case OSMP_INT: // int
        {
            break;
        }
        case OSMP_LONG: // long int
        {
            break;
        }
        case OSMP_UNSIGNED_CHAR: // unsinged char
        {
            break;
        }
        case OSMP_UNSIGNED: // unsigned int
        {
            break;
        }
        case OSMP_UNSIGNED_LONG: // unsigned long int
        {
            break;
        }
        case OSMP_FLOAT: // float
        {
            break;
        }
        case OSMP_DOUBLE: // double
        {
            break;
        }
        case OSMP_BYTE: // char
        {
            break;
        }
        default:
            break;
    }
    return OSMP_SUCCESS;
}
int OSMP_Finalize(void)
{
    int returnVal;
    int out = OSMP_SUCCESS;
    struct stat shmStat;

    returnVal = fstat(OSMP_ShmFileDescriptor, &shmStat);
    if(returnVal != OSMP_ERROR)
    {
        returnVal = munmap(OSMP_ShmPtr, (size_t)shmStat.st_size);
        if(returnVal == OSMP_ERROR)
        {
            out = OSMP_ERROR;
        }
    }
    else
    {
        out = OSMP_ERROR;
    }

    free(sharedMemoryName);
    return out;
}

char* OSMP_GetShmName(pid_t pid)
{
    char szProcessID[32];
    if(sprintf(szProcessID, "%d", pid) <= 0)
    {
        return NULL;
    }
    sharedMemoryName = calloc(strlen(OSMP_SHMEM_NAME) + strlen(szProcessID) + 1, (sizeof(char)));

    if(sprintf(sharedMemoryName, "%s%s", OSMP_SHMEM_NAME, szProcessID) <= 0)
    {
        free(sharedMemoryName);
        return NULL;
    }

    return sharedMemoryName;
}