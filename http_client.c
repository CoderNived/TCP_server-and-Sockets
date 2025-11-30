/* http_client.c - simple HTTP client using getaddrinfo */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>           // getaddrinfo, addrinfo
#include <arpa/inet.h>       // inet_ntop

#define HOST "example.com"
#define PORT "80"

int main(void)
{
    int s;
    struct addrinfo hints, *res, *p;
    char request[1024];
    char buffer[4096];
    ssize_t bytes_sent, bytes_received;

    printf("Resolving host %s...\n", HOST);

    // 1. Prepare hints for getaddrinfo
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_STREAM;    // TCP

    int status = getaddrinfo(HOST, PORT, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return 1;
    }

    // 2. Loop through the results and connect to the first we can
    for (p = res; p != NULL; p = p->ai_next) {
        // Create socket
        s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s < 0) {
            perror("socket");
            continue;
        }

        // Print which IP we’re trying
        char ipstr[INET6_ADDRSTRLEN];
        struct sockaddr_in *addr = (struct sockaddr_in *)p->ai_addr;
        inet_ntop(AF_INET, &addr->sin_addr, ipstr, sizeof(ipstr));
        printf("Trying %s:%s...\n", ipstr, PORT);

        // Try to connect
        if (connect(s, p->ai_addr, p->ai_addrlen) == 0) {
            printf("✅ Connected to %s:%s\n", ipstr, PORT);
            break;  // success
        }

        perror("connect failed");
        close(s);
    }

    if (p == NULL) {
        fprintf(stderr, "Failed to connect to %s:%s\n", HOST, PORT);
        freeaddrinfo(res);
        return 1;
    }

    // 3. Build HTTP GET request
    snprintf(request, sizeof(request),
             "GET / HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n"
             "\r\n",
             HOST);

    printf("Sending HTTP request...\n");
    bytes_sent = send(s, request, strlen(request), 0);
    if (bytes_sent < 0) {
        perror("send");
        close(s);
        freeaddrinfo(res);
        return 1;
    }
    printf("Sent %zd bytes\n", bytes_sent);

    // 4. Receive and print response
    printf("---- Response start ----\n");
    while ((bytes_received = recv(s, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        printf("%s", buffer);
    }
    printf("\n---- Response end ----\n");

    if (bytes_received < 0) {
        perror("recv");
    }

    // 5. Cleanup
    close(s);
    freeaddrinfo(res);
    return 0;
}
