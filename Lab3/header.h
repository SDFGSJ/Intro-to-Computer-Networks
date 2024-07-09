#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#define PACKET_LOSS_RATE 0.1

// in congestion control, we don't need l3 information
typedef struct Segment {
    uint32_t ack_num;
    uint32_t seq_num;
    bool loss;
} Segment;

bool packet_loss();
int mymin(int a, int b);