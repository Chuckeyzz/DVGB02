#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

int main()
{
    /* Declarations */
    struct sockaddr_in serveraddr;
    int sd;
    char buf[13];

    /* Create socket */
    sd = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);

    /* Clear address structure */
    memset(&serveraddr, 0,
    sizeof(struct sockaddr_in));

    /* Add address family */
    serveraddr.sin_family = AF_INET;

    /* Add IP address of server*/

    inet_pton(AF_INET, "127.0.0.1",
    &serveraddr.sin_addr);

    /* Add the port number */
    serveraddr.sin_port = htons(2009);

    /* Connect */
    connect(sd,
    (struct sockaddr*)&serveraddr,
    sizeof(struct sockaddr_in));

    /* Send data */
    write(sd, "Hello world!", 12 );

    /* Read data */
    read(sd, buf, 12 );

    /* add string end sign, write to screen*/
    buf[12] = '\0';
    printf("%s\n", buf);

    /* Close socket */
    close(sd);
}
