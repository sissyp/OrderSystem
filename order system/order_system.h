#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define Torderlow 1
#define Torderhigh 5
#define Norderlow 1
#define Norderhigh 5 
#define Tprep 1
#define Tbake 10
#define Tlow 5
#define Thigh 15

void *order(void *x); 

int main(int argc, char** argv);
