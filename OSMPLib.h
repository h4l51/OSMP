//
// Created by Felix Halstrup on 08.04.21.
//

#ifndef DEMO_OSMPLIB_H
#define DEMO_OSMPLIB_H

#define OSMP_SUCCESS    0
#define OSMP_ERROR      -1

typedef int OSMP_Datatype;

int OSMP_Init(int *argc, char ***argv);
int OSMP_Size(int *size);
int OSMP_Rank(int *rank);
int OSMP_Send(const void *buf, int count, OSMP_Datatype datatype, int dest);
int OSMP_Recv(void *buf, int count, OSMP_Datatype datatype, int *source, int *len);
int OSMP_Finalize(void);

#endif //DEMO_OSMPLIB_H
