#include "header.h"
int received[200010];
/*
 * @description
 * Task1
 * 1. create a socket and connect to server.
 *    (server's IP address = "127.0.0.1")
 *    (server's Port number = 45525)
 * 2. Receive 1 message from server and print it out.
 *    (The message you sent from server)
 *
 * Task2
 * Continuously receive data from Server and send back ACK.
*/
int main(int argc , char *argv[])
{
    //Create socket.
    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(socket_fd==-1){
        printf("Failed to create TCP scoket\n");
    }

    //Set up server's address.
    struct sockaddr_in server_addr;
    server_addr.sin_family = PF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(45525);

    //Connect to server's socket.
    int error=connect(socket_fd,(struct sockaddr*)&server_addr,sizeof(server_addr));
    if(error==-1){
        printf("Failed to connect to server\n");
    }

    // Receive 1 message and print it out.
    int sent_cnt = 0, rcv_cnt = 0;
    char welcome[500];
    memset(welcome, 0, sizeof(welcome));
    rcv_cnt = recv(socket_fd, welcome, 24, 0);
    //printf("receive welcome msg %d chars\n",rcv_cnt);
    printf("%s...\n", welcome);

    // Receive data and send ACK.
    struct Segment rcv_seg; //receive from server
    struct Segment send_seg;    //send to server
    int loss_idx = 0;
    while(1){
        recv(socket_fd, &rcv_seg, sizeof(Segment), 0);
        send_seg.seq_num = rcv_seg.seq_num;
        send_seg.loss = packet_loss();
        if(send_seg.loss){
            loss_idx = mymin(loss_idx, rcv_seg.seq_num);
            printf("Loss: seq_num = %d\n", rcv_seg.seq_num);
            send_seg.ack_num = loss_idx;
        }else{
            received[rcv_seg.seq_num] = 1;  //buffer this segment
            while(received[loss_idx]){  //move to next un-received packet
                loss_idx++;
            }
            printf("Received: seq_num = %d\n", rcv_seg.seq_num);
            send_seg.ack_num = loss_idx;
        }
        send(socket_fd, &send_seg, sizeof(Segment), 0);
    }
    return 0;
}