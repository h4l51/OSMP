//
// Created by Felix Halstrup on 08.04.21.
//

#ifndef DEMO_OSMPLIB_H
#define DEMO_OSMPLIB_H

#define OSMP_SUCCESS    0
#define OSMP_ERROR      -1

#define OSMP_SHORT short int
#define OSMP_INT int
#define OSMP_LONG long int
#define OSMP_UNSIGNED_CHAR unsigned char
#define OSMP_UNSIGNED unsigned int
#define OSMP_UNSIGNED_LONG unsigned long int
#define OSMP_FLOAT float
#define OSMP_DOUBLE double
#define OSMP_BYTE char

typedef int OSMP_Datatype;

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

#endif //DEMO_OSMPLIB_H
