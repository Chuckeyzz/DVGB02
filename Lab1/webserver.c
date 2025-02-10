#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define PORT 8080 //the assigned port for the sockets
#define BUFFER_SIZE 1024 //the size of the packages that will be sent
#define WEB_ROOT "./Website" //root folder of the index.html file

/*function that returns the filetype based on its extension.
It checks for what comes after the '.' and tries to match it to common extensions and then
return that mime type so the browser knows how to process the file.
If the extension is not known it returns "application/octet-stream" that indicates that the file
is not intended to be displayed in the browser*/
const char *get_mime_type(const char *path) {
    const char *ext = strrchr(path, '.');
    if (!ext) return "application/octet-stream";
    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".jpg") == 0) return "image/jpeg";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".js") == 0) return "application/javascript";
    return "application/octet-stream";
}

void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];

    //We store the client request here, in this case a http request from a browser, also check for errors
    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("read");
        close(client_fd);
        return;
    }
    //we will also place a null terminator at the end of the read request
    buffer[bytes_read] = '\0';

    // Parse HTTP request
    char method[16], path[256], protocol[16];
    //We store the different parts of the http request in these arrays
    //method stores for example "GET" or "POST"
    //path stores the requested resource path
    //protocol stores the protocol version eg. "HTTP/1.1"
    sscanf(buffer, "%s %s %s", method, path, protocol);

    // Only handle GET requests
    if (strcmp(method, "GET") != 0) { //Checking if the request is not a GET and then proceed to close the connection
        const char *response = "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
        write(client_fd, response, strlen(response));
        close(client_fd);
        return;
    }

    // Construct file path
    char file_path[512]; //array to store the path to the requested file
    //we add together the WEB_ROOT and the path in the request for a complete path at file_path
    snprintf(file_path, sizeof(file_path), "%s%s", WEB_ROOT, path);
    //if the request does not point to a file we add the "index.html" to the path so the client lands there default
    if (file_path[strlen(file_path) - 1] == '/') {
        strcat(file_path, "index.html");
    }

    // Open and read the requested file
    int file_fd = open(file_path, O_RDONLY); //this attempts to open the file at file_path in read only mode

    //Here we determin the size of the requested file and save it to file_size
    struct stat file_stat;
    fstat(file_fd, &file_stat);
    size_t file_size = file_stat.st_size;

    // Determine MIME type
    const char *mime_type = get_mime_type(file_path);

    // Send HTTP response headers
    char header[BUFFER_SIZE];
    int header_length = snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: %zu\r\n"
        "Content-Type: %s\r\n"
        "\r\n",
        file_size, mime_type);
    write(client_fd, header, header_length);

    // Send file content
    ssize_t bytes_sent;
    //the while loop will keep executing as long as read() reads more than 0 bytes from the requested file
    while ((bytes_read = read(file_fd, buffer, sizeof(buffer))) > 0) {
        bytes_sent = write(client_fd, buffer, bytes_read); //write the data to the clients socket
        if (bytes_sent < 0) {
            perror("write");
            break;
        }
    }

    close(file_fd);
    close(client_fd);
}

int main() {
    int server_fd, client_fd; //declaring file descriptor variables for the server socket and client sockets
    struct sockaddr_in server_addr, client_addr; //declaring variables for holding the ipv4 addresses and port numbers of the server and the client
    socklen_t client_addr_len = sizeof(client_addr); //prepare a variable to hold the max length of a client address

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0); //creates the socked and returns the outcome to the fd
    if (server_fd < 0) { //if the outcome is -1 it means the socket was not created correctly then we show an error message and terminate the program
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* Set socket options
    we enable the socket option SO_REUSEADDR here on the server socket so that we can reuse 
    the server socket even if we quickly restart the server, else the socket might be in a wait
    state we get the error that the adress is in use*/
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind socket to port
    server_addr.sin_family = AF_INET; //set the server address to use the ipv4 protocol
    server_addr.sin_addr.s_addr = INADDR_ANY; //allow the socket to bind to any network interface
    server_addr.sin_port = htons(PORT); //set the port number for the socket

    /*here we bind the socket to the specified address and port and directly checks if the
    binding process returns -1, if it does we terminate the program with an error code*/
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    /* Listen for incoming connections.
    this transitions the socket to the passive listening state where it can
    queue incoming connection requests, we also have error checks for 
    if the transition returns -1*/
    if (listen(server_fd, SOMAXCONN) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on http://localhost:%d\n", PORT);

    // Main loop to accept and handle client connections
    while (1) {
        //If there is a connection request add the returned file descriptor to client_fd
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        //we also check for errors
        if (client_fd < 0) {
            perror("accept");
            continue;
        }
        //we call handle_client() if we get a successful connection request
        handle_client(client_fd);
    }

    close(server_fd);
    return 0;
}
