# Simple TCP Server & Client in C

This project contains a **basic TCP server** and a **TCP client** written in C.  
They demonstrate how to:

- Create sockets
- Bind to an IP/port (server)
- Listen and accept connections (server)
- Connect to a server (client)
- Send and receive messages over TCP

The code is intended to be run on **Linux / WSL (Ubuntu on Windows)**.

---

## 🗂 Files

(Use the names you actually used; example names:)

- `TCP_server.c` – TCP server code  
- `TCP_client.c` – TCP client code  

Both use the same port:

```c
#define PORT 8080
and typically run on:

c
Copy code
SERVER_IP = "127.0.0.1"   // localhost
🧠 How the TCP Server Works (TCP_server.c)
High-level flow:

Create a socket

c
Copy code
server_fd = socket(AF_INET, SOCK_STREAM, 0);
AF_INET → IPv4

SOCK_STREAM → TCP

Set socket options (optional but recommended)

c
Copy code
setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
This avoids Address already in use when restarting the server quickly.

Fill sockaddr_in structure

c
Copy code
server_addr.sin_family = AF_INET;
server_addr.sin_addr.s_addr = INADDR_ANY;   // listen on all interfaces
server_addr.sin_port = htons(PORT);         // convert port to network byte order
Bind the socket to the IP and port

c
Copy code
bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
This tells the OS: “I want to receive TCP connections on this port”.

Listen for incoming connections

c
Copy code
listen(server_fd, BACKLOG);
Puts the socket into “listening mode”. BACKLOG is the size of pending connection queue.

Accept a client

c
Copy code
client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
Blocks until a client connects.

Returns a new socket (client_fd) used for talking to that client.

Read data from the client

c
Copy code
bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
buffer[bytes_read] = '\0';
printf("Message from client: %s\n", buffer);
Send a response back

c
Copy code
const char *response = "Hello from server";
write(client_fd, response, strlen(response));
Close the connection

c
Copy code
close(client_fd);   // close client socket
// optionally loop back to accept() to handle next client
Shutdown server when done

c
Copy code
close(server_fd);
🧠 How the TCP Client Works (TCP_client.c)
High-level flow:

Create a socket

c
Copy code
s = socket(AF_INET, SOCK_STREAM, 0);
Fill the server address

c
Copy code
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(PORT);
server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // localhost
Connect to the server

c
Copy code
connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr));
Send a message to the server

c
Copy code
const char *message = "Hello from client";
send(s, message, strlen(message), 0);
Receive server response

c
Copy code
bytes_received = recv(s, buffer, sizeof(buffer) - 1, 0);
buffer[bytes_received] = '\0';
printf("Response from server: %s\n", buffer);
Close the socket

c
Copy code
close(s);
🛠 Prerequisites
GCC installed
On Ubuntu / WSL:

bash
Copy code
sudo apt update
sudo apt install build-essential
A terminal (WSL / Linux terminal)

▶️ How to Compile
Open a terminal in the folder containing the files:

bash
Copy code
cd "/mnt/c/Users/nived/OneDrive/Desktop/TCP Server"
Compile the server:
bash
Copy code
gcc TCP_server.c -o TCP_server
Compile the client:
bash
Copy code
gcc TCP_client.c -o TCP_client
If there are no errors, you’ll have two executables:

TCP_server

TCP_client

▶️ How to Run (Server + Client)
1️⃣ Start the server (Terminal 1)
bash
Copy code
./TCP_server
You should see something like:

text
Copy code
Server listening on port 8080
Waiting for a new client...
Keep this terminal open – the server is now waiting for client connections.

2️⃣ Start the client (Terminal 2)
Open another terminal in the same folder:

bash
Copy code
cd "/mnt/c/Users/nived/OneDrive/Desktop/TCP Server"
./TCP_client
Expected output (client side):

text
Copy code
Creating socket...
Connecting to server 127.0.0.1:8080...
Connected.
Sending message to server: Hello from client
Response from server: Hello from server
Socket closed. Exiting client.
Expected output (server side):

text
Copy code
Server listening on port 8080
Waiting for a new client...

Connection accepted from 127.0.0.1:xxxxx
Message from client: Hello from client
Sent response to client: Hello from server
Client disconnected.

Waiting for a new client...
You can run the client multiple times; the server will handle each connection one by one.

🧪 Testing with nc (netcat) instead of C client (optional)
You can also test the server using nc:

bash
Copy code
nc 127.0.0.1 8080
Then type:

text
Copy code
Hello server
Press Enter — you should see Hello from server as a reply.

🐞 Common Errors & Fixes
❌ Bind failed: Address already in use
Another process is already using the port (8080), or the previous server didn’t shut down cleanly.

Wait a few seconds and try again, or

Change the port in the code (e.g., 9090) and recompile, or

Make sure you added:

c
Copy code
setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
❌ Connection to the server failed (client side)
Server is not running → start ./TCP_server first.

Wrong IP/port in client → verify:

c
Copy code
SERVER_IP = "127.0.0.1";
PORT      = 8080;
📌 Notes
This is a blocking, single-process server: it handles one client at a time.

For handling many clients concurrently, you can use:

fork() (process per client),

pthread (thread per client), or

select() / poll() / epoll (I/O multiplexing).

This project is meant as a starting point to understand raw TCP sockets in C.

perl
Copy code


