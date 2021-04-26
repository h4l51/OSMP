//
// Created by Felix Halstrup on 08.04.21.
//
#ifndef OSMPLIB_H
#define OSMPLIB_H

#define OSMP_SUCCESS    0
#define OSMP_ERROR      -1

typedef void* OSMP_Request;

// maximale Zahl der Nachrichten pro Prozess
#define OSMP_MAX_MESSAGES_PROC 16

// maximale Anzahl der Nachrichten, die insgesamt vorhanden sein dürfen
#define OSMP_MAX_SLOTS 256

// maximale Länge der Nutzlast einer Nachricht
#define OSMP_MAX_PAYLOAD_LENGTH 1024

//shared memory name
#define OSMP_SHMEM_NAME "OSMPSHM_"



/*
typedef short int OSMP_SHORT;
typedef int OSMP_INT;
typedef long int OSMP_LONG;
typedef unsigned char OSMP_UNSIGNED_CHAR;
typedef unsigned int OSMP_UNSIGNED;
typedef unsigned long int OSMP_UNSIGNED_LONG;
typedef float OSMP_FLOAT;
typedef double OSMP_DOUBLE;
typedef char OSMP_BYTE;
 */

typedef enum {
    OSMP_SHORT = 1,
    OSMP_INT,
    OSMP_LONG,
    OSMP_UNSIGNED_CHAR,
    OSMP_UNSIGNED,
    OSMP_UNSIGNED_LONG,
    OSMP_FLOAT,
    OSMP_DOUBLE,
    OSMP_BYTE
} OSMP_Datatype;

int OSMP_Init(int *argc, char ***argv);  /// Initializes the OSMP environment and enables access to the shared memory
int OSMP_Size(int *size); /// Returns the number of OSMP processes, excluding the starter process
int OSMP_Rank(int *rank); /// Returns the OSMP Processnumber of the calling process
int OSMP_Send(const void *buf, int count, OSMP_Datatype datatype, int dest); /// Sends a message
/// \param buf Starting address of the puffer with the message
/// \param count Amount of the elements from the given type
/// \param datatype OSMP-Type of the data
/// \param dest Number of the receiver
/// \return
int OSMP_Recv(void *buf, int count, OSMP_Datatype datatype, int *source, int *len); /// Receives a message
int OSMP_Finalize(void); /// Frees the shared memory

#endif // OSMPLIB_H
