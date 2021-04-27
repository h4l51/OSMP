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
OSMP_shm_info* infoStruct;

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

    infoStruct = OSMP_ShmPtr;
    return OSMP_SUCCESS;
}

int OSMP_Size(int *size)
{
    *size = infoStruct->nProcessCount;
    return OSMP_SUCCESS;
}
int OSMP_Rank(int *rank)
{
    pid_t pid = getpid();
    for(int i = 0; i < infoStruct->nProcessCount; i++)
    {
        if(infoStruct->nProcessRank[i][0] == pid)
        {
            *rank = infoStruct->nProcessRank[i][1];
            return OSMP_SUCCESS;
        }
    }
    return OSMP_ERROR;
}
int OSMP_Send(const void *buf, int count, OSMP_Datatype datatype, int dest)
{
    OSMP_Message* pMessageDest = OSMP_GetFirstMessagePointer(dest);

    for(int i = 0; i < infoStruct->nMessagesPerProcess; i++)
    {
        if(pMessageDest->buffer[0] == '\0')
        {
            break;
        }
        else if(i == infoStruct->nMessagesPerProcess - 1)
        {
            return OSMP_ERROR; // no available space
        }
        pMessageDest += 1;
    }

    switch(datatype)
    {
        case OSMP_BYTE: // char
        {
            int rank, returnVal;
            returnVal = OSMP_Rank(&rank);
            if(returnVal == OSMP_ERROR)
                return OSMP_ERROR;
            pMessageDest->nSenderID = rank;
            pMessageDest->nReceiverID = dest;
            memcpy(pMessageDest->buffer, buf, (size_t)count);
            break;
        }
        case OSMP_INT: // int
        {

            break;
        }
        case OSMP_SHORT: // short int
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

        default:
            break;
    }
    return OSMP_SUCCESS;
}
int OSMP_Recv(void *buf, int count, OSMP_Datatype datatype, int *source, int *len)
{
    int rank, returnVal;
    returnVal = OSMP_Rank(&rank);
    if(returnVal == OSMP_ERROR)
        return OSMP_ERROR;

    OSMP_Message* pMessageDest = OSMP_GetFirstMessagePointer(rank);
    *source = pMessageDest->nSenderID;

    size_t bufferSize = sizeof(pMessageDest->buffer) / sizeof(pMessageDest->buffer[0]);
    if(bufferSize > count)
        bufferSize = (size_t)count;

    switch(datatype)
    {
        case OSMP_BYTE: // char
        {
            *len = (int)bufferSize;
            memcpy(buf, pMessageDest->buffer, bufferSize);
            break;
        }
        case OSMP_INT: // int
        {
            break;
        }
        case OSMP_SHORT: // short int
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

        default:
            break;
    }
    for(int i = 0; i < infoStruct->nMessagesPerProcess - 1; i++)
    {
        memcpy(pMessageDest + (unsigned int)i, pMessageDest + (unsigned int)(i+1),sizeof(OSMP_Message)); // move messages, last read gets overwritten
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

OSMP_Message* OSMP_GetFirstMessagePointer(int nOSMPProcess)
{
    int* pInfoEnd = (int*)infoStruct;
    pInfoEnd += (2 + (2 * infoStruct->nProcessCount)); // end of info struct

    OSMP_Message* tmpPointer = (OSMP_Message*)pInfoEnd;


    for(int i = 0; i < nOSMPProcess; i++)
    {
        tmpPointer += infoStruct->nMessagesPerProcess;
    }
    return tmpPointer;
}