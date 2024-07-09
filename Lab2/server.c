#include "header.h"
/*
 * @description
 * In this function, You need to implement the server side
 * that sends messages to and from the client.
 *
 *        Enter ""test" to start or "quit" to leave!"
 * Server ----------------------------------------> Client
 *        Receive "test" or "quit" or {wrong input} from client
 * Server <---------------------------------------- Client
 *        test: Read text file from input and send test segment.
 *        quit: End loop.
 *        {wrong input}: Server send "Wrong input! Enter "test" to start or "quit" to leave!"
 * Server ----------------------------------------> Client
 *
 * @param
 * clientfd: The socket descriptor used for sending/receiving data
 *           with a client that has connected to server.
 * fileName: The file name read from sample input. e.g. "sample_input1.txt"
*/
void serverFunction(int clientfd, char* fileName){
    Segment s;
    readFile(&s, fileName);
    //printf("src ip = %s\n",s.l3info.SourceIpv4);
    printSegment(&s);
    int send_val = send(clientfd, &s, sizeof(s), 0);
    //printf("send val = %d\n",send_val);
}

/*
 * @description
 * Task1
 * 1. Create a TCP socket bind to port 45525.
 * 2. Listen the TCP socket.
 * 3. Accept the connect and get the Client socket file descriptor.
 * 4. Send 1 message "Hi, I'm server {Your_student_ID}" to client.
 * 5. Go finish the client.c TASK 1
 *
 * Task2
 * 1. Pass the client fd and fileName into serverFunction()
 *    Example:
 *        serverFunction(client_fd, argv[1])
*/
int main(int argc , char *argv[]){
    //Create TCP socket
    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(socket_fd==-1){
        printf("Failed to create TCP scoket\n");
    }

    //Set up server's address.
    struct sockaddr_in server_addr;
    server_addr.sin_family = PF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(45525);

    //Bind socket to the address.
    bind(socket_fd,(struct sockaddr*)&server_addr,sizeof(server_addr));

    //Listening the socket.
    listen(socket_fd,5);

    //Accept the connect request.
    struct sockaddr_in client_info;
    int addrlen=sizeof(client_info);
    int client_socket_fd=accept(socket_fd, (struct sockaddr*)&client_info, &addrlen);
    printf("New connection\n");

    int sent_cnt = 0, rcv_cnt = 0;

    //Send message to client.
    char* welcome = "Hi, I'm server 109062318";   //len = 24
    //printf("welcome strlen = %ld\n",strlen(welcome));

    sent_cnt = send(client_socket_fd, welcome, strlen(welcome), 0);
    //printf("welcome msg sent %d chars\n",sent_cnt);

    char* msg = "Enter 'test' to start or 'quit' to leave!";   //initial system message, len = 41
    //printf("system msg strlen = %ld\n",strlen(msg));
    sent_cnt = send(client_socket_fd, msg, strlen(msg), 0);    //send system message to client
    //printf("system msg sent %d chars\n",sent_cnt);

    char op[100];   //receive user input operation
    while(1){
        memset(op, 0, sizeof(op));
        rcv_cnt = recv(client_socket_fd, op, sizeof(op), 0);  //receive input operation from client
        //printf("[debug]server receive op %ld chars\n",strlen(op));

        if(strcmp(op, "quit") == 0){
            //printf("[debug] server receive quit\n");
            break;
        }else if(strcmp(op, "test") == 0){
            //printf("[debug] server receive test\n");

            char* receive_msg = "Receive Data from server...";
            //printf("receive msg strlen = %ld\n",strlen(receive_msg));
            sent_cnt = send(client_socket_fd, receive_msg, strlen(receive_msg), 0);
            //printf("receive msg sent %d chars\n",sent_cnt);

            serverFunction(client_socket_fd, argv[1]);  //cant put this before send message
        }else{
            //printf("[debug] server receive wrong input %s\n",op);
            char* wrong_msg = "Wrong input! Enter 'test' to start or 'quit' to leave!";    //system message when received wrong input
            //printf("wrong msg strlen = %ld\n",strlen(wrong_msg));
            sent_cnt = send(client_socket_fd, wrong_msg, strlen(wrong_msg), 0);
            //printf("wrong msg sent %d chars\n",sent_cnt);
        }
    }

    close(socket_fd);
    return 0;
}