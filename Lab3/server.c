#include "header.h"
int acked[200010];
int duplicate[3], dup_idx;
/*
 * @description
 * Write your server's send function here.
 *
 * 1. Send cwnd sequences of data start with correct sequence number.
 * 2. Simulate packet loss.
 *
 * @param
 * You can increase or decrease parameters by yourself.
 * Some useful information can be used either as global varaible or passed as parameters to functions:
 * 1. client_fd: The socket descriptor used for sending/receiving data
 *               with a client that has connected to server.
 * 2. last_acked: Previous acknowledge packets.
 * 3. cwnd: congestion window size
*/
void server_send(int client_fd, int last_acked, int cwnd) {
    struct Segment seg;
    for(int i = last_acked+1, cnt = 0;cnt < cwnd;i++){
        if(!acked[i]){
            seg.seq_num = i;
            seg.ack_num = 0;
            seg.loss = 0;
            printf("Send: seq_num = %d\n", seg.seq_num);
            send(client_fd, &seg, sizeof(Segment), 0);
            cnt++;
        }
    }
}

/*
 * @description
 * Write your server's receive function here.
 * ** We don't need to consider time-out in this lab.
 *    loss will only occur when packet_loss() = true. **
 * 1. Receive ACKs from client.
 * 2. Detect if 3 duplicate ACK occurs.
 * 3. Update cwnd and ssthresh
 *
 *
 * @param
 * You can increase or decrease parameters by yourself.
 * Some useful information can be used either as global varaible or passed as parameters to functions:
 * 1. client_fd: The socket descriptor used for sending/receiving data
 *               with a client that has connected to server.
 * 2. last_acked: Previous acknowledge packets.
 * 3. ssthresh: slow start threshold
 * 4. cwnd: congestion window size
*/
int server_receive(int client_fd, int* last_acked, int ssthresh, int cwnd) {
    struct Segment rcv_seg; //receive from client
    int rcv_cnt = 0, dupli_ack_flag = 0;
    for(int i = *last_acked+1 ; i <= *last_acked + cwnd ; i++){
        recv(client_fd, &rcv_seg, sizeof(Segment), 0);
        printf("ACK: ack_num = %d\n", rcv_seg.ack_num);
        if(!rcv_seg.loss){
            acked[rcv_seg.seq_num]=1;   //client successfully received
        }

        duplicate[dup_idx] = rcv_seg.ack_num;
        if(!dupli_ack_flag && duplicate[0] == duplicate[1] && duplicate[1] == duplicate[2]){
            printf("3 duplicate ACKs : ACK_num = %d, ssthresh = %d\n", rcv_seg.ack_num, ssthresh);
            dupli_ack_flag = 1;
        }
        dup_idx = (dup_idx + 1) % 3;
    }
    *last_acked = rcv_seg.ack_num-1;
    return dupli_ack_flag;
}

/*
 * @description
 * Task1
 * 1. Create a TCP socket bind to port 45525.
 * 2. Listen the TCP socket.
 * 3. Accept the connect and get the Client socket file descriptor
 * 4. Send 1 message "Hi I'm server {your_student_id}" to client
 * 5. Go finish the client.c TASK 1
 *
 * Task2:
 * Start data transmission with simulating congestion control.
 *
 * To be simple, we receive ACKs only after all cwnd sequences of data are sent,
 * so we cannot react to 3-duplicate ACKs immediately. This is ok for this lab.
 * Ex:
 *      while(ROUND--) {
 *          server_send();
 *          server_receive();
 *      }
*/
int main(int argc, char* argv[]) {
    // Create TCP socket
    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(socket_fd == -1){
        printf("Failed to create TCP scoket\n");
    }

    // Set up server's address.
    struct sockaddr_in server_addr;
    server_addr.sin_family = PF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(45525);

    //Bind socket to the address.
    bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    //Listening the socket.
    listen(socket_fd, 5);

    //Accept the connect request.
    struct sockaddr_in client_info;
    int addrlen = sizeof(client_info);
    int client_socket_fd = accept(socket_fd, (struct sockaddr*)&client_info, &addrlen);
    printf("New connection\n");

    // Send 1 message to client.
    int sent_cnt = 0, rcv_cnt = 0;
    char* welcome = "Hi, I'm server 109062318";   //len = 24
    //printf("welcome strlen = %ld\n",strlen(welcome));
    sent_cnt = send(client_socket_fd, welcome, strlen(welcome), 0);
    //printf("welcome msg sent %d chars\n",sent_cnt);

    // Start congestion control
    int ROUND = 15, last_acked = -1, ssthresh = 8, cwnd = 1, dup_happened = 0;
    while(ROUND--){
        printf("=====\n");
        if(cwnd<ssthresh){
            printf("State: slow start (cwnd = %d, ssthresh = %d)\n",cwnd,ssthresh);
        }else{
            printf("State: congestion avoidance (cwnd = %d, ssthresh = %d)\n",cwnd,ssthresh);
        }

        //printf("Last acked = %d\n",last_acked);
        server_send(client_socket_fd, last_acked, cwnd);
        dup_happened = server_receive(client_socket_fd, &last_acked, ssthresh, cwnd);

        if(dup_happened){
            ssthresh = (cwnd==1) ? 1 : cwnd/2;
            cwnd = 1;
        }else{
            if(cwnd<ssthresh){
                cwnd<<=1;
            }else{
                cwnd++;
            }
        }
    }

    // Close the socket.
    close(socket_fd);
    return 0;
}