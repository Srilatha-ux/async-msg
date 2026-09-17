async-msg — Project Progress Log
1. Project Goal

async-msg is a Linux-based C++ messaging system designed to demonstrate:

Modern C++20
Linux system programming
TCP networking
TCP message framing
JSON-based application protocols
Multithreading
Thread pools
Synchronization
Client/server architecture
Error handling
Logging
Unit testing
Integration testing
CMake
GDB debugging
Git and GitHub

The project is developed inside WSL2 Ubuntu using VS Code.

2. Development Environment
Host OS: Windows
Linux environment: WSL2 Ubuntu
Compiler: GCC
Language: C++20
Build system: CMake
Version control: Git
Repository: GitHub

The application is being developed and tested from the Linux environment.

3. Project Structure
async-msg/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   └── async_msg/
├── src/
├── apps/
├── tests/
├── docs/
└── scripts/
Directory responsibilities

apps/
: Application entry points.

src/
: Implementation of application components.

include/
: Public headers and interfaces.

tests/
: Unit and protocol test programs.

docs/
: Project documentation and learning notes.

scripts/
: Utility and test scripts that may be added later.

4. Milestone 1 — C++20 Build System
Objective

Create a minimal C++20 executable and establish the CMake build system.

Important concepts
CMake

CMake generates the build system used to compile the project.

The project uses:

CMakeLists.txt
    ↓
cmake
    ↓
build/
    ↓
server executable
C++20

The project explicitly requires C++20.

The CMake configuration contains:

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

This ensures the project is compiled using the C++20 language standard without compiler-specific extensions.

Build commands
cmake -S . -B build
cmake --build build

Run the server:

./build/server

Initial output:

async-msg server starting..

5. Milestone 2 — Git Repository

Git was configured for source-code version control.

The build directory is excluded using:

build/

This prevents generated build files from being committed to the repository.

Useful commands:

git status
git add .
git commit -m "message"
git push

The project uses GitHub as the remote repository.

6. Milestone 3 — TCP Server
Objective

Create a Linux TCP server capable of accepting client connections.

The server uses the Linux socket API.

Basic flow:

socket()
   ↓
bind()
   ↓
listen()
   ↓
accept()
   ↓
client connection
Important system calls
socket()

Creates a socket endpoint.

The server uses:

socket(AF_INET, SOCK_STREAM, 0);

AF_INET
: IPv4 networking.

SOCK_STREAM
: TCP byte-stream socket.

bind()

Associates the socket with a local IP address and port.

The project uses port:

8080
listen()

Places the socket into listening mode so that incoming connections can be accepted.

accept()

Accepts an incoming client connection and returns a new file descriptor representing that client.

The original listening socket continues accepting new connections.

7. TCP Testing

The server was tested using:

nc 127.0.0.1 8080

nc (netcat) was used as a simple TCP client.

The server successfully detected client connections.

This demonstrated the basic TCP connection lifecycle.

8. Milestone 4 — Multiple Messages

The server was extended from handling a single message to repeatedly receiving data from a connected client.

The architecture became:

accept client
     ↓
receive data
     ↓
process data
     ↓
receive more data
     ↓
client disconnects

The server also supports multiple sequential client connections.

9. Important TCP Concept — TCP Is a Byte Stream

TCP does not preserve application-level message boundaries.

For example, the sender might perform:

send("hello")
send("world")

The receiver is not guaranteed to receive:

hello
world

It could receive:

helloworld

or:

hel
lowor
ld

Therefore, the application needs its own message framing protocol.

This is one of the key networking concepts demonstrated by this project.

10. Milestone 5 — Length-Prefixed Message Framing

The project uses:

[4-byte length][payload]

Example:

+----------------+----------------------+
| 4-byte length  | JSON/message payload |
+----------------+----------------------+

If the payload is 15 bytes:

[00 00 00 0F][15 bytes of payload]

The receiver first reads exactly four bytes.

It decodes the length.

Then it reads exactly that many payload bytes.

11. send_all()

A normal send() call does not guarantee that all requested bytes are transmitted.

Therefore the project implements:

bool send_all(
    int socket_fd,
    const void* data,
    std::size_t size
);

Conceptually:

send_all()
    ↓
send some bytes
    ↓
check how many were sent
    ↓
send remaining bytes
    ↓
repeat

This handles partial writes.

12. recv_all()

The same problem exists with recv().

A single recv() cannot be assumed to return all requested bytes.

The project therefore implements:

RecvResult recv_all(
    int socket_fd,
    void* data,
    std::size_t size
);

The function keeps receiving until:

all requested bytes arrive
the client disconnects
an error occurs
13. RecvResult

Originally recv_all() returned:

bool

That was insufficient because two different situations could occur:

client disconnected
        vs.
receive error

The protocol layer was therefore changed to:

enum class RecvResult
{
    Success,
    Disconnected,
    Error
};

This allows the caller to distinguish the result.

14. Network Byte Order

The protocol uses a 4-byte integer for message length.

The project uses:

htonl()

when sending/encoding the length:

std::uint32_t encode_length(std::uint32_t length);

and:

ntohl()

when decoding:

std::uint32_t decode_length(
    std::uint32_t encoded_length
);

This provides a defined byte order for data transmitted over the network.

15. Message Size Validation

The server limits the maximum payload to:

4096 bytes

The rules are:

Payload	Result
0 bytes	Reject
1 byte	Accept
4096 bytes	Accept
4097 bytes	Reject

This protects the server from blindly allocating memory based on an untrusted length field.

16. Protocol Testing

A test program was created:

tests/protocol_test.cpp

It tests:

Length encoding
Length decoding
send_all()
recv_all()
Successful transmission

The test uses:

socketpair(AF_UNIX, SOCK_STREAM, 0, sockets);

This creates two connected sockets locally without requiring a TCP server.

The test currently reports:

Protocol test passed.
17. Integration Testing With a Protocol Client

A small client program was created:

tests/protocol_client.cpp

It connects to:

127.0.0.1:8080

and sends multiple length-prefixed messages.

The client sends:

hello async-msg
Second Message
hello hey

The server successfully receives the individual framed messages.

18. Boundary Testing

The client was also used to test a payload of:

4096 bytes

The server accepted it.

A payload of:

4097 bytes

was rejected with:

Message too large: 4097 bytes

This verifies the maximum-message boundary.

19. Current Architecture

The current system is approximately:

             TCP
Client ─────────────────→ Server
  │                         │
  │  [length][payload]      │
  │                         │
  └────────────────────────→│
                            │
                       recv_all()
                            │
                       decode length
                            │
                       validate size
                            │
                       recv_all()
                            │
                       process payload

The protocol layer currently contains:

encode_length()
decode_length()
send_all()
recv_all()


