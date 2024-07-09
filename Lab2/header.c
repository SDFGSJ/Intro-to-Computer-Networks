#include"header.h"

void printHeader(char* header);
void printSegment(Segment* s);
void readFile(Segment* test, char* fileName);

void printHeader(char* header){
    printf("The header is:\n");
    for(int i=0;i<20;i++){
        printf("%02X ",(uint8_t)header[i]);
    }
    printf("\n");
}

void printSegment(Segment* s){
    printf("Receive information:\n\n\
            Layer 3 information:\n\
            Source IP: %s , Destination Ip: %s \n\
            Protocol: %u (TCP)\n\n\
            Layer 4 information:\n\
            Source port: %u , Destination port: %u \n\
            Seq number: %u , Ack number: %u \n\
            Header length: %u (bytes) , FLAG: 0x%X (ACK) \n\
            Window size: %u \n\n",s->l3info.SourceIpv4,s->l3info.DestIpv4,s->l3info.protocol,\
            s->l4info.SourcePort,s->l4info.DestPort,s->l4info.SeqNum,\
            s->l4info.AckNum,s->l4info.HeaderLen,s->l4info.Flag,s->l4info.WindowSize);
}

/*
 * @description
 * This function opens a text file for reading.
 *
 * @param
 * test: The testcase of the information for you to create TCP header.
 * fileName: The file name read from sample input. e.g. "sample_input1.txt"
*/
void readFile(Segment* test, char* fileName) {
    memset(test,0,sizeof(Segment));
    char buffer[1024];

    FILE* file = fopen(fileName, "r");
    fgets(buffer, sizeof(buffer), file);

    /*printf("buffer = %s, len = %d\n",buffer,strlen(buffer));
    printf("%d%d\n",buffer[12]=='\n',buffer[13]=='\0');
    for(int i=0;i<128;i++){
        if(buffer[12]==i){
            printf("ascii code=%d\n",i);
        }
    }
    printf("idx = %d\n",strcspn(buffer+1, "\n"));*/

    buffer[strcspn(buffer/*+1*/, "\n")] = '\0';
    memcpy(test->l3info.SourceIpv4, buffer, sizeof(test->l3info.SourceIpv4));

    fgets(buffer, sizeof(buffer), file);
    buffer[strcspn(buffer, "\n")] = '\0';
    memcpy(test->l3info.DestIpv4, buffer, sizeof(test->l3info.DestIpv4));

    test->l3info.protocol = 6;

    fgets(buffer, sizeof(buffer), file);
    sscanf(buffer, "%u", &test->l4info.SourcePort);

    fgets(buffer, sizeof(buffer), file);
    sscanf(buffer, "%u", &test->l4info.DestPort);

    fgets(buffer, sizeof(buffer), file);
    sscanf(buffer, "%u", &test->l4info.SeqNum);

    fgets(buffer, sizeof(buffer), file);
    sscanf(buffer, "%u", &test->l4info.AckNum);
    test->l4info.Flag = 0x10;
    test->l4info.HeaderLen = 5;

    fgets(buffer, sizeof(buffer), file);
    sscanf(buffer, "%u", &test->l4info.WindowSize);
}
