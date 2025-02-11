#include <string.h>
#include <stdio.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define TIME_PORT 37
#define BUF_SIZE 1

int main(){
    int sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    unsigned int current_time;
    time_t unix_time;
    
    //create a UDP socket and assign it to sock as a file descriptor
    sock = socket(AF_INET, SOCK_DGRAM, 0); //SOCK_DGRAM for UDP

    // we set up the server address structure
    memset(&server_addr, 0, sizeof(server_addr)); //we set the memory of server_addr to 0s
    server_addr.sin_family = AF_INET; //Ipv4
    server_addr.sin_addr.s_addr = INADDR_ANY; //make sure the socket can use any network interface
    server_addr.sin_port = htons(TIME_PORT);  // Ensure port is in network byte order

    // bind the socket to the address and port
    bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

    printf("Time server listening on UDP port %d\n", TIME_PORT);

    //start looping to handle incoming requests
    while (1) {
        char buffer[BUF_SIZE];
        client_len = sizeof(client_addr); //length of the clients address
        
        //receive data from the client
        recvfrom(sock, buffer, BUF_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);

        //get current Unix time
        unix_time = time(NULL);
         //2208988800 is a constant representing the amount of seconds between 1900-1970 we add these seconds to get the amount of seconds since 1900(RFC 868 time)
        current_time = (unsigned int)(unix_time + 2208988800); 
        //convert to network byte order before sending
        current_time = htonl(current_time);

        //send the 32-bit time back to the client
        sendto(sock, &current_time, sizeof(current_time), 0, (struct sockaddr *)&client_addr, client_len);
    }

    close(sock);
    return 0;
}