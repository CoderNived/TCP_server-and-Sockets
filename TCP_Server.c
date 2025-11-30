/* srv.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BACKLOG 5          // how many pending connections queue will hold

int main(void)
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addrlen;
    char buffer[1024];
    const char *response = "Hello from server";

    // 1. Create a TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2. Allow reuse of address (avoids "Address already in use" on restart)
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 3. Zero out server_addr and set fields
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;    // listen on all interfaces
    server_addr.sin_port = htons(PORT);

    // 4. Bind socket to the specified IP and port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 5. Start listening for incoming connections
    if (listen(server_fd, BACKLOG) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("✅ Server listening on port %d\n", PORT);

    // 6. Main loop: accept and handle clients one by one
    while (1) {
        addrlen = sizeof(client_addr);
        memset(&client_addr, 0, sizeof(client_addr));

        printf("\nWaiting for a new client...\n");

        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
        if (client_fd < 0) {
            perror("Accept failed");
            // don't exit server, just continue waiting
            continue;
        }

        printf("📥 Connection accepted from %s:%d\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));

        // 7. Read data from client
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read < 0) {
            perror("Read failed");
            close(client_fd);
            continue;
        }

        buffer[bytes_read] = '\0';   // null-terminate
        printf("📨 Message from client (%zd bytes): %s\n", bytes_read, buffer);

        // 8. Send response back to client
        ssize_t bytes_sent = write(client_fd, response, strlen(response));
        if (bytes_sent < 0) {
            perror("Write failed");
            close(client_fd);
            continue;
        }

        printf("📤 Sent response to client: %s\n", response);

        // 9. Close connection with this client
        close(client_fd);
        printf("🔌 Client disconnected.\n");
    }

    // (Unreachable in this infinite loop unless you add a break/exit logic)
    close(server_fd);
    return 0;
}
