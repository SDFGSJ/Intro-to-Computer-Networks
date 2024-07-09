#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

typedef struct l4info{
    uint32_t AckNum,SeqNum,SourcePort,DestPort,Flag,HeaderLen,WindowSize;
}L4info;

typedef struct l3info{
    char SourceIpv4[16], DestIpv4[16];
    uint32_t protocol;
}L3info;

typedef struct Segment{
    char header[20];
    char pseudoheader[12];
    L3info l3info;
    L4info l4info;
}Segment;


void printHeader(char* header);
void printSegment(Segment* s);
void readFile(Segment* test, char* fileName);