#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define TIME_PORT 37

int main(){
    int sock;
    struct sockaddr_in server_addr;
    unsigned int received_time;
    socklen_t addr_len;

    //create a UDP socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    // set up the server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; //use IPv4
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // server address
    server_addr.sin_port = htons(TIME_PORT);  // Port in network byte order

    // send an empty datagram to the time server
    sendto(sock, NULL, 0, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Wait for the server to send the 32-bit time
    addr_len = sizeof(server_addr); //get the size of server_addr
    //get the time from the server and save it to receieved_time
    recvfrom(sock, &received_time, sizeof(received_time), 0, (struct sockaddr *)&server_addr, &addr_len);

    //convert the received time from network byte order
    received_time = ntohl(received_time);
    //2208988800 is a constant representing the amount of seconds between 1900-1970 that we subtract to get the seconds from 1970(unix time), we also add an hour to land on CET instead of GMT
    time_t unix_time = (time_t)(received_time - 2208988800 + 3600); 

    //print the time
    printf("Current time: %s", ctime(&unix_time));

    close(sock);
    return 0;
}
