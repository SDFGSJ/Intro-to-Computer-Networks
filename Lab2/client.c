#include "header.h"
/*
 * @description
 * Write your own function to create header.
 *
 * @param
 * s: The test segment sent from server
*/
void myheadercreater(Segment* s){
    uint64_t pseudo_sum = 0;
    uint32_t mask = (1<<16) - 1;

    uint32_t src_ip = ntohl(inet_addr(s->l3info.SourceIpv4));    //"10.5.4.107"
    //printf("src ip len = %ld\n",strlen(s->l3info.SourceIpv4));
    //printf("src ip in hex = %X\n",src_ip);
    for(int i=1;i>=0;i--){
        pseudo_sum += ((src_ip >> (i*16)) & mask);
        //printf("%X\n",(src_ip >> (i*16)) & mask);
    }

    uint32_t dst_ip = ntohl(inet_addr(s->l3info.DestIpv4));    //"10.8.9.237"
    //printf("dst ip in hex = %X\n",dst_ip);
    for(int i=1;i>=0;i--){
        pseudo_sum += ((dst_ip >> (i*16)) & mask);
        //printf("%X\n",(dst_ip >> (i*16)) & mask);
    }

    pseudo_sum+=s->l3info.protocol;
    pseudo_sum+=20;

    pseudo_sum+=s->l4info.SourcePort;
    pseudo_sum+=s->l4info.DestPort;
    pseudo_sum+=(s->l4info.SeqNum>>16);
    pseudo_sum+=(s->l4info.SeqNum & mask);
    pseudo_sum+=(s->l4info.AckNum>>16);
    pseudo_sum+=(s->l4info.AckNum & mask);
    pseudo_sum+=((s->l4info.HeaderLen << 12) + s->l4info.Flag);
    pseudo_sum+=s->l4info.WindowSize;

    uint32_t over = pseudo_sum>>16;
    //printf("over = %d\n",over);
    pseudo_sum += over;
    uint16_t checksum = ~pseudo_sum;
    //printf("checksum = %X\n",checksum);

    //fill the header
    *((uint16_t*)(s->header+0)) = ntohs(s->l4info.SourcePort);
    *((uint16_t*)(s->header+2)) = ntohs(s->l4info.DestPort);
    *((uint32_t*)(s->header+4)) = ntohl(s->l4info.SeqNum);
    *((uint32_t*)(s->header+8)) = ntohl(s->l4info.AckNum);
    *((uint8_t*)(s->header+12)) = s->l4info.HeaderLen << 4;
    *((uint8_t*)(s->header+13)) = s->l4info.Flag;
    *((uint16_t*)(s->header+14)) = ntohs(s->l4info.WindowSize);
    *((uint16_t*)(s->header+16)) = htons(checksum);
}


/*
 * @description
 * In this function, You need to implement the client side
 * that sends messages to and from the server.
 *
 *        Receive the message: "Enter "test" to start or "quit" to leave!"
 * Server ----------------------------------------> Client
 *        client type: "test" or "quit" or {wrong input}
 * Server <---------------------------------------- Client
 *        test: 1. Receive "Receive Data from server..." from server
 *              2. receive test segment from server.
 *        quit: Close the socket.
 *        {wrong input}: do nothing
 * Server ----------------------------------------> Client
 *
 * @param
 * sockfd: The socket descriptor used for sending/receiving data
 *           with a client that has connected to server.
 * s     : The test segment sent from server
*/
void receiveData(int sockfd,Segment* s){
    memset(s, 0, sizeof(*s));
    int recv_value = recv(sockfd, s, sizeof(*s), 0);
    //printf("recv value = %d\n",recv_value);
    printSegment(s);
}


/*
 * @description
 * Task1
 * 1. create a socket and connect to server.
 *    (server's IP address = "127.0.0.1")
 *    (server's Port number = 45525)
 * 2. Receive message: "Hi, I'm server {Your_student_ID}..." from server and print it out.
 *    (The message you sent from server)
 *
 * Task2
 * 1. Instantiate a Segment
 * 2. Pass your socket_fd and the instance of Segment into the receiveData()
 * 3. Write your own function to create header.
 * 4. Print the header.
 *    Example:
 *        Segment s;
 *        receiveData(sock_fd,&s);
 *        myheadercreater(&s);
 *        printHeader(s.header);
*/
int main(int argc , char *argv[])
{
    //Create TCP socket.
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

    int sent_cnt = 0, rcv_cnt = 0;
    //Receive message from server and print it out.
    char welcome[500];
    memset(welcome, 0, sizeof(welcome));
    rcv_cnt = recv(socket_fd, welcome, 24, 0);
    //printf("receive welcome msg %d chars\n",rcv_cnt);
    printf("%s...\n", welcome);

    char buf[500];
    memset(buf, 0, sizeof(buf));
    rcv_cnt = recv(socket_fd, buf, sizeof(buf), 0);  //receive system message from server
    //printf("receive system msg %d chars\n",rcv_cnt);
    printf("server: %s\n",buf);

    char op[100];   //user input operation
    while(1){
        scanf("%s",op);
        sent_cnt = send(socket_fd, op, strlen(op), 0);   //send user input operation to server
        //printf("op msg sent %d chars\n", sent_cnt);
        if(strcmp(op, "quit") == 0){
            //printf("[debug] client send quit\n");
            break;
        }else if(strcmp(op, "test") == 0){
            //printf("[debug] client send test\n");

            memset(buf, 0, sizeof(buf));
            rcv_cnt = recv(socket_fd, buf, sizeof(buf), 0);  //receive input message from server
            //printf("receive input msg %d chars\n",rcv_cnt);
            printf("%s\n",buf);

            Segment s;
            receiveData(socket_fd, &s);
            myheadercreater(&s);
            printHeader(s.header);
        }else{
            //printf("[debug] client send wrong input\n");

            memset(buf, 0, sizeof(buf));
            rcv_cnt = recv(socket_fd, buf, 54, 0);  //receive wrong input message from server
            //printf("receive wrong msg %d chars\n",rcv_cnt);
            printf("server: %s\n",buf);
        }
    }

    close(socket_fd);
    printf("close Socket\n");
    return 0;
}
//https://pubs.opengroup.org/onlinepubs/7908799/xns/syssocket.h.html
//https://pubs.opengroup.org/onlinepubs/7908799/xns/arpainet.h.html
//https://github.com/davidleitw/socket