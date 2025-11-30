/* socket_client.c */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define PORT 80                    // HTTP port
#define SERVER_IP "93.184.216.34"  // example.com
#define HOST_NAME "example.com"    // used in HTTP Host header

int main(void)
{
    int s;
    struct sockaddr_in server_addr;
    char request[1024];
    char buffer[4096];
    ssize_t bytes_sent, bytes_received;

    printf("Creating socket...\n");

    // 1. Create socket
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    printf("Filling server address...\n");

    // 2. Fill server address struct
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    printf("Connecting to server %s:%d...\n", SERVER_IP, PORT);

    // 3. Connect to the server
    if (connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        perror("Connection to the server failed");
        close(s);
        exit(EXIT_FAILURE);
    }

    printf("✅ Connected to %s:%d\n", SERVER_IP, PORT);

    // 4. Build HTTP request
    snprintf(request, sizeof(request),
             "GET / HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n"
             "\r\n",
             HOST_NAME);

    printf("Sending HTTP request...\n");

    // 5. Send the HTTP request
    bytes_sent = send(s, request, strlen(request), 0);
    if (bytes_sent < 0) {
        perror("send() failed");
        close(s);
        exit(EXIT_FAILURE);
    }

    printf("Sent %zd bytes\n", bytes_sent);
    printf("Receiving response...\n");
    printf("---- Response start ----\n");

    // 6. Receive response
    while ((bytes_received = recv(s, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        printf("%s", buffer);
    }

    printf("\n---- Response end ----\n");

    if (bytes_received < 0) {
        perror("recv() failed");
    }

    // 7. Close socket
    close(s);
    printf("Socket closed. Exiting.\n");

    return 0;
}
