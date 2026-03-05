# Simple TCP Client-Server Implementation in C using POSIX Sockets

> *A clean, educational implementation of TCP socket programming in C — covering the complete client-server lifecycle from socket creation to graceful teardown.*

<div align="center">

![Language](https://img.shields.io/badge/language-C-A8B9CC?style=for-the-badge&logo=c)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20WSL-FCC624?style=for-the-badge&logo=linux)
![License](https://img.shields.io/badge/license-MIT-green?style=for-the-badge)
![Contributions Welcome](https://img.shields.io/badge/contributions-welcome-blueviolet?style=for-the-badge)
![Sockets](https://img.shields.io/badge/POSIX-sockets-orange?style=for-the-badge)

</div>

---

## 📌 Project Overview

This repository contains a minimal but complete TCP client-server implementation written in C using the POSIX sockets API. It demonstrates the fundamental mechanics of network communication that underpin virtually every networked application in existence.

### Why TCP Sockets Matter

TCP (Transmission Control Protocol) is the backbone of reliable network communication. Every time you load a webpage, query a database, or make an API call, you are using a TCP connection. Understanding how sockets work at the systems level gives you a foundational mental model that applies regardless of the language, framework, or protocol stack you work with.

### Why Learn Socket Programming in C

The POSIX socket API — designed in the 1980s and still in active use — exposes network I/O at its lowest practical level. Learning socket programming in C teaches you:

- How the operating system exposes network I/O through file descriptors
- How connection state is managed between two hosts
- What frameworks and libraries like Node.js, Python's `asyncio`, and Nginx do under the hood
- How to reason about blocking vs non-blocking I/O at the kernel level

### Where TCP Client-Server Architecture Appears in Real Systems

| System | Role of TCP |
|---|---|
| **HTTP / HTTPS** | Every web request is a TCP connection to port 80 or 443 |
| **PostgreSQL / MySQL** | Database clients connect to servers over TCP |
| **Redis** | In-memory store accessed via TCP on port 6379 |
| **Nginx / Apache** | Accept thousands of TCP connections concurrently |
| **gRPC / Thrift** | RPC frameworks built on top of TCP streams |
| **SSH** | Secure shell sessions tunneled over TCP |
| **Multiplayer games** | Real-time state sync over persistent TCP connections |

---

## 🎓 Learning Objectives

By studying and running this code, you will develop a working understanding of:

- **POSIX Socket API** — `socket()`, `bind()`, `listen()`, `accept()`, `connect()`, `send()`, `recv()`, `close()`
- **TCP connection lifecycle** — three-way handshake, data transfer, teardown
- **IPv4 addressing and port binding** — how `sockaddr_in` structures map to network addresses
- **Client-server architecture** — the asymmetry between a passive server and an active client
- **Blocking I/O model** — how system calls block the calling thread until data is available
- **Socket options** — using `SO_REUSEADDR` to avoid `Address already in use` errors
- **Error handling** — proper pattern for checking return values of socket system calls

---

## 🗂️ Repository Structure

```
TCP-Socket-Example/
│
├── TCP_server.c       # TCP server — binds, listens, accepts, responds
├── TCP_client.c       # TCP client — connects, sends, receives
└── README.md
```

### File Descriptions

**`TCP_server.c`**
Implements a single-threaded TCP server that binds to `127.0.0.1:8080`, listens for an incoming connection, reads a message from the client, sends a response, and closes the connection. Demonstrates the passive/server side of the socket API.

**`TCP_client.c`**
Implements a TCP client that connects to `127.0.0.1:8080`, sends a message to the server, reads the server's response, and closes the socket. Demonstrates the active/client side of the socket API.

---

## 📡 TCP Networking Concepts Explained

### Socket

A **socket** is an endpoint for communication — a file descriptor returned by the kernel that you can read from and write to like any other file. Under the hood, it is backed by kernel buffers and a network stack. On Linux, everything is a file: a socket is no different from `stdin` or a regular file descriptor in how you interact with it.

### TCP vs UDP

| Property | TCP | UDP |
|---|---|---|
| Connection | Connection-oriented (handshake) | Connectionless |
| Reliability | Guaranteed delivery, in-order | No guarantees |
| Error checking | Yes (retransmission) | Checksum only |
| Speed | Slower (overhead) | Faster (no overhead) |
| Use cases | HTTP, SSH, databases | DNS, video streaming, gaming |

TCP guarantees that bytes arrive in the order they were sent and retransmits lost segments automatically. This makes it ideal for any protocol where correctness matters more than raw speed.

### IPv4 Addressing

An IPv4 address is a 32-bit number written in dotted-decimal notation (e.g., `127.0.0.1`). The address `127.0.0.1` is the **loopback address** — it always refers to the local machine and never leaves the network interface. This is why both programs in this repo can run on the same machine and communicate.

### Port Numbers

A port is a 16-bit integer (0–65535) that multiplexes connections on a single IP address. The kernel uses the `(IP, Port)` pair to route incoming packets to the correct socket. Port `8080` is a conventional alternative HTTP port, commonly used for local development.

### Client-Server Architecture

In this model, the **server** is passive — it binds to a known address and waits for connections. The **client** is active — it initiates a connection to the server's known address. This asymmetry is fundamental: clients need to know where servers are, but servers do not need to know where clients are in advance.

---

## 🖥️ Server Architecture

The server follows a strict sequential lifecycle. Each system call must succeed before the next is attempted.

### Step-by-Step Workflow

**Step 1 — Create the socket**

```c
int server_fd = socket(AF_INET, SOCK_STREAM, 0);
```

`AF_INET` specifies IPv4. `SOCK_STREAM` specifies TCP (as opposed to `SOCK_DGRAM` for UDP). The return value is a file descriptor — a non-negative integer — or `-1` on failure.

**Step 2 — Set socket options**

```c
int opt = 1;
setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
```

`SO_REUSEADDR` allows the server to immediately rebind to the same port after a restart. Without this, you will encounter `Address already in use` during development because the kernel holds the port in `TIME_WAIT` state for up to 60 seconds after the connection closes.

**Step 3 — Bind to an address and port**

```c
struct sockaddr_in address;
address.sin_family      = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;
address.sin_port        = htons(8080);

bind(server_fd, (struct sockaddr*)&address, sizeof(address));
```

`bind()` associates the socket with a local address. `htons()` (host-to-network short) converts the port from host byte order to network byte order (big-endian), which is required by the protocol.

**Step 4 — Listen for incoming connections**

```c
listen(server_fd, 3);
```

`listen()` transitions the socket from `CLOSED` to `LISTEN` state. The second argument (backlog = 3) specifies the maximum number of pending connections the kernel will queue before refusing new ones.

**Step 5 — Accept a client connection**

```c
int addrlen = sizeof(address);
int client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
```

`accept()` **blocks** until a client connects. When a client arrives, the kernel completes the three-way TCP handshake and returns a **new** file descriptor (`client_fd`) representing that specific connection. The original `server_fd` remains available to accept further connections.

**Step 6 — Read the client's message**

```c
char buffer[1024] = {0};
int bytes_read = read(client_fd, buffer, 1024);
printf("Client: %s\n", buffer);
```

**Step 7 — Send a response**

```c
char *response = "Message received";
send(client_fd, response, strlen(response), 0);
```

**Step 8 — Close the connection**

```c
close(client_fd);
close(server_fd);
```

Both file descriptors must be closed. `client_fd` triggers a TCP `FIN` to the client, initiating the four-way teardown handshake.

---

## 💻 Client Architecture

The client's lifecycle is simpler — it does not bind or listen. It only needs to connect to a known server address.

### Step-by-Step Workflow

**Step 1 — Create the socket**

```c
int sock = socket(AF_INET, SOCK_STREAM, 0);
```

**Step 2 — Configure the server address**

```c
struct sockaddr_in serv_addr;
serv_addr.sin_family = AF_INET;
serv_addr.sin_port   = htons(8080);

inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
```

`inet_pton()` (presentation-to-network) converts the human-readable IP string `"127.0.0.1"` into its 32-bit binary representation in network byte order.

**Step 3 — Connect to the server**

```c
connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
```

`connect()` initiates the TCP three-way handshake. It blocks until the connection is established or fails. On success, the socket is fully connected and ready for I/O.

**Step 4 — Send a message**

```c
char *message = "Hello from client";
send(sock, message, strlen(message), 0);
```

**Step 5 — Receive the server's response**

```c
char buffer[1024] = {0};
int bytes_received = read(sock, buffer, 1024);
printf("Server: %s\n", buffer);
```

**Step 6 — Close the socket**

```c
close(sock);
```

---

## 🔄 Communication Flow Diagram

```
Client                          Server
  │                               │
  │                               │  socket()
  │                               │  bind()
  │                               │  listen()
  │                               │       │
  │       ── SYN ──────────────>  │       │ (three-way
  │       <─ SYN-ACK ──────────   │       │  handshake)
  │       ── ACK ──────────────>  │  accept()
  │                               │
  │  connect() returns            │
  │                               │
  │  send("Hello from client")    │
  │  ── data ─────────────────>   │  read() → "Hello from client"
  │                               │
  │                               │  send("Message received")
  │  read() → "Message received"  │
  │  <── data ─────────────────   │
  │                               │
  │  close()                      │
  │  ── FIN ───────────────────>  │
  │  <── FIN-ACK ──────────────   │  close()
  │  ── ACK ───────────────────>  │
  │                               │
```

**Explanation of each phase:**

1. **Server setup** — The server calls `socket()`, `bind()`, and `listen()` before the client starts. This is why the server must always be started first.
2. **Three-way handshake** — Initiated by `connect()` on the client side. The kernel handles SYN / SYN-ACK / ACK automatically; your application code does not see these packets.
3. **Data transfer** — After the handshake, both sides can freely send and receive. TCP guarantees the data arrives in order.
4. **Four-way teardown** — Initiated by `close()` on the client. Each side sends a FIN and must acknowledge the other's FIN before the connection is fully closed.

---

## 🔧 Prerequisites

### Required Environment

| Requirement | Details |
|---|---|
| **OS** | Linux (Ubuntu, Debian, Arch) or WSL 2 on Windows |
| **Compiler** | GCC 9+ or Clang |
| **Terminal** | Two terminal windows (one for server, one for client) |

### Installing GCC on Ubuntu / WSL

```bash
sudo apt update
sudo apt install build-essential
```

Verify the installation:

```bash
gcc --version
# gcc (Ubuntu 11.x.x) 11.x.x
```

`build-essential` installs GCC, G++, Make, and the standard C library headers — everything needed to compile this project.

---

## 🔨 Compilation Instructions

Compile each program separately:

```bash
# Compile the server
gcc TCP_server.c -o TCP_server

# Compile the client
gcc TCP_client.c -o TCP_client
```

**What these commands do:**

- `gcc` — invokes the GNU C Compiler
- `TCP_server.c` — the C source file to compile
- `-o TCP_server` — names the output binary `TCP_server` (instead of the default `a.out`)

No additional linker flags are needed — the POSIX socket API (`sys/socket.h`, `netinet/in.h`, `arpa/inet.h`) is part of the C standard library on Linux.

For stricter compilation with warnings enabled (recommended during development):

```bash
gcc -Wall -Wextra -o TCP_server TCP_server.c
gcc -Wall -Wextra -o TCP_client TCP_client.c
```

---

## ▶️ Running the Programs

The server **must be started before the client**. Use two separate terminal windows.

### Terminal 1 — Start the Server

```bash
./TCP_server
```

**Expected output:**

```
Waiting for connections on port 8080...
Connection accepted from client
Client message: Hello from client
Response sent. Connection closed.
```

The server will block at `accept()` until a client connects. It prints nothing until a client arrives.

### Terminal 2 — Run the Client

```bash
./TCP_client
```

**Expected output:**

```
Connected to server at 127.0.0.1:8080
Message sent: Hello from client
Server response: Message received
Connection closed.
```

After the client exits, the server handles the disconnection and also exits (in this single-connection implementation).

---

## 🔬 Testing with Netcat (Optional)

You can test the server without running the client binary using `netcat` — a general-purpose TCP/UDP utility available on all Linux systems.

**Start the server:**

```bash
./TCP_server
```

**In a second terminal, connect with netcat:**

```bash
nc 127.0.0.1 8080
```

Type a message and press Enter. The server will receive it and send back a response.

```
Hello from netcat
Message received
```

### Why Netcat Is Useful

Netcat is the standard tool for ad-hoc TCP debugging. It lets you:

- Verify a server is listening on a port without writing any client code
- Inspect raw server responses
- Test protocol behavior interactively
- Simulate a client connection from a shell script

You can also reverse the test — use netcat as a server to test your client:

```bash
# In terminal 1 — start netcat as a listening server
nc -l 8080

# In terminal 2 — run your client
./TCP_client
```

---

## ⚠️ Common Errors and Troubleshooting

### `bind failed: Address already in use`

**Cause:** Another process (possibly a previous run of the server) is still holding port 8080, or the kernel is holding it in `TIME_WAIT`.

**Fix:**

```bash
# Find and kill the process using port 8080
sudo lsof -i :8080
kill -9 <PID>

# Or wait ~60 seconds for TIME_WAIT to expire
# SO_REUSEADDR in the server code should prevent this in most cases
```

---

### `connect: Connection refused`

**Cause:** The client is trying to connect but no server is listening on `127.0.0.1:8080`.

**Fix:** Ensure the server is started and running before the client. Check for compilation errors in `TCP_server.c`.

---

### `Segmentation fault`

**Cause:** Most commonly, using an uninitialized socket file descriptor or passing `NULL` where a valid pointer is expected.

**Fix:** Always check the return value of `socket()`, `bind()`, `listen()`, `accept()`, and `connect()`. Any return value of `-1` indicates an error; use `perror()` to print the system error message:

```c
if (server_fd < 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
}
```

---

### `read() returns 0 unexpectedly`

**Cause:** The remote side has closed the connection. `read()` returning 0 means EOF — the peer sent a FIN.

**Fix:** Treat a `read()` return value of 0 as a signal to close the socket and stop reading. This is correct behavior, not an error.

---

### Server exits immediately without waiting for a client

**Cause:** `listen()` or `accept()` failed silently due to unchecked errors.

**Fix:** Add return value checks after every system call and use `perror()` to surface the actual kernel error.

---

## 📋 Limitations of This Implementation

This code is intentionally minimal and is designed for learning, not production use. The following limitations are by design:

| Limitation | Description |
|---|---|
| **Single client** | The server handles exactly one client connection then exits |
| **Blocking I/O** | Every `read()` and `accept()` call blocks the calling thread indefinitely |
| **No concurrency** | While one client is being served, all other connections are queued or dropped |
| **No protocol** | Messages have no length prefix or delimiter — stream boundaries are undefined |
| **No error recovery** | A failed `send()` or `recv()` terminates the program |
| **Fixed buffer size** | Messages longer than 1024 bytes will be truncated |

### How Real Systems Solve These Problems

- **Apache (pre-fork model):** `fork()` a new child process per accepted connection
- **Nginx (event-driven model):** A single thread uses `epoll` to handle thousands of connections without blocking
- **Node.js:** Non-blocking I/O with a libuv event loop
- **Go:** Lightweight goroutines — each connection gets its own goroutine, the runtime multiplexes them onto OS threads

---

## 🛠️ Possible Improvements

The following extensions are well-defined next steps for anyone who wants to go deeper:

**Concurrency**

```
• Multi-client with fork()            — one process per client (Apache model)
• Multi-threaded with pthread         — one thread per client, shared memory
• select() / poll()                   — single-threaded multiplexing (portable)
• epoll()                             — Linux-native, high-performance event loop
```

**Robustness**

```
• Length-prefixed protocol            — define a message format to handle partial reads
• Graceful shutdown handling          — catch SIGINT, close all open sockets cleanly
• Timeout handling                    — use SO_RCVTIMEO to prevent infinite blocking
• Logging system                      — structured logs with timestamps and client IDs
```

**Features**

```
• Persistent connections              — keep the connection alive for multiple messages
• TLS encryption                      — wrap the socket with OpenSSL for secure comms
• IPv6 support                        — use AF_INET6 and sockaddr_in6
• Broadcast / multicast               — explore UDP-based group communication
```

Implementing `select()` or `epoll()` is particularly valuable — it is the fundamental mechanism behind every high-performance I/O framework.

---

## 🌐 Real-World Applications

TCP client-server architecture is not an academic abstraction. It is the literal foundation of the modern internet:

**Web Servers (HTTP/HTTPS)**
Every browser request is a TCP connection. Nginx accepts the connection, reads the HTTP request (plain text over TCP), and writes an HTTP response back on the same socket.

**Relational Databases**
PostgreSQL listens on port 5432. Your ORM opens a TCP connection, sends SQL queries as bytes, and reads result sets as a stream of bytes — framed by the PostgreSQL wire protocol.

**Chat Applications**
WhatsApp, Slack, and IRC all maintain persistent TCP connections between clients and servers. The server reads messages from one client's socket and writes them to another's.

**Microservices**
gRPC (used by Google, Netflix, and Kubernetes) encodes RPC calls as Protocol Buffers and transmits them over HTTP/2, which is itself a multiplexed protocol built on TCP.

**Multiplayer Games**
Game servers accept TCP connections from players, maintain state (positions, health, scores), and broadcast updates. Latency-sensitive games sometimes switch to UDP for speed, sacrificing delivery guarantees.

---

## 👨‍💻 Author

**Nived Shenoy**

Electronics & Telecommunication Engineering Student
Interested in Systems Programming, Networking, and Distributed Systems.

[![GitHub](https://img.shields.io/badge/GitHub-CoderNived-181717?style=for-the-badge&logo=github)](https://github.com/CoderNived)

---

## 📄 License

This project is licensed under the **MIT License**.

```
MIT License

Copyright (c) 2024 Nived Shenoy

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
```

---

<div align="center">

*Understanding sockets is understanding the internet. Everything else is abstraction built on top of this.*

⭐ **Star this repository if it helped you understand TCP networking.**

</div>
