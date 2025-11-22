# 42_Minitalk

![Score](https://img.shields.io/badge/Score-125%2F100-success)
![Language](https://img.shields.io/badge/Language-C-blue)
![Norm](https://img.shields.io/badge/Norm-passing-brightgreen)

## 📋 Table of Contents
- [Description](#description)
- [Project Overview](#project-overview)
- [How It Works](#how-it-works)
- [Project Structure](#project-structure)
- [Implementation Details](#implementation-details)
- [Compilation](#compilation)
- [Usage](#usage)
- [Bonus Features](#bonus-features)
- [Technical Challenges](#technical-challenges)
- [Testing](#testing)
- [Author](#author)

## 🔍 Description

**Minitalk** is a communication program that allows data exchange between a client and a server using **UNIX signals**. This project at 42 School teaches inter-process communication (IPC), signal handling, and bit manipulation.

The challenge is to transmit strings from a client process to a server process using only two signals: `SIGUSR1` and `SIGUSR2`.

This implementation successfully passed all tests and earned **125/100** points, including all bonus features.

## 🎯 Project Overview

### Objectives

- Create a **server** that:
  - Displays its PID on startup
  - Receives and displays strings from multiple clients
  - Never crashes or exits unexpectedly
  
- Create a **client** that:
  - Takes the server PID and a string as parameters
  - Sends the string to the server using signals
  - Receives acknowledgment from the server (bonus)

### Key Concepts

- ✅ **UNIX Signals** - Inter-process communication using `SIGUSR1` and `SIGUSR2`
- ✅ **Bit Manipulation** - Converting characters to binary and vice versa
- ✅ **Signal Handlers** - Processing asynchronous signals
- ✅ **Process Communication** - Client-server architecture
- ✅ **Memory Management** - Dynamic buffer handling

## ⚙️ How It Works

### Signal-Based Communication

Since we can only use two signals (`SIGUSR1` and `SIGUSR2`), we transmit data bit by bit:

- `SIGUSR1` = bit `1`
- `SIGUSR2` = bit `0`

### Transmission Process

```
Character: 'A' (ASCII 65)
Binary: 01000001

Client sends 8 signals:
SIGUSR2 → 0
SIGUSR1 → 1
SIGUSR2 → 0
SIGUSR2 → 0
SIGUSR2 → 0
SIGUSR2 → 0
SIGUSR2 → 0
SIGUSR1 → 1

Server reconstructs: 01000001 → 'A'
```

### Visual Workflow

```
┌─────────────────────────────────────────────────────────┐
│                    SERVER PROCESS                        │
│  1. Start and display PID                               │
│  2. Wait for signals (pause loop)                       │
│  3. Receive bit by bit (SIGUSR1/SIGUSR2)               │
│  4. Reconstruct character (8 bits)                      │
│  5. Build message in buffer                             │
│  6. Display complete message (on '\0')                  │
│  7. Send acknowledgment to client (bonus)               │
└─────────────────────────────────────────────────────────┘
                            ↑
                     SIGUSR1/SIGUSR2
                            ↓
┌─────────────────────────────────────────────────────────┐
│                    CLIENT PROCESS                        │
│  1. Get server PID and message                          │
│  2. Convert each character to binary                    │
│  3. Send 8 signals per character                        │
│  4. Add delay between signals (usleep)                  │
│  5. Send null terminator                                │
│  6. Wait for server acknowledgment (bonus)              │
└─────────────────────────────────────────────────────────┘
```

## 📁 Project Structure

```
42_mini-talk/
├── Makefile              # Compilation rules
├── mini.h                # Header with structures and prototypes
├── server.c              # Server implementation
├── client.c              # Client implementation
├── utilis.c              # Utility functions (buffer management)
├── Libft/                # Personal C library
│   ├── libft.a
│   └── ...
└── Printf/               # Custom printf implementation
    ├── libftprintf.a
    └── ...
```

### File Breakdown

#### `mini.h`
- Global structure definition (`t_server_state`)
- Function prototypes
- Required includes

#### `server.c`
- Server initialization
- Signal handler (`handle_sigusr`)
- Bit reception and character reconstruction
- Message display and acknowledgment

#### `client.c`
- Client initialization
- Message transmission (`send_message`)
- Bit-by-bit sending (`send_byte`)
- Acknowledgment reception (bonus)

#### `utilis.c`
- `buffer_join()` - Dynamic buffer management for incoming characters

## 🔧 Implementation Details

### Server State Structure

```c
typedef struct s_server_state
{
    int     g_bit_count;     // Current bit position (0-7)
    int     g_client_pid;    // PID of current client
    char    g_char;          // Character being reconstructed
    char    *buffer;         // Message buffer
    char    *new;            // Temporary buffer for reallocation
}   t_server_state;
```

### Server Signal Handler

```c
void handle_sigusr(int sig, siginfo_t *info, void *context)
{
    // Save client PID on first bit
    if (g_state.g_bit_count == 0)
        g_state.g_client_pid = info->si_pid;
    
    // Shift left and add new bit
    g_state.g_char = (g_state.g_char << 1) | (sig == SIGUSR1);
    g_state.g_bit_count++;
    
    // Complete byte received (8 bits)
    if (g_state.g_bit_count == 8)
    {
        if (g_state.g_char == 0)  // End of message
        {
            // Send acknowledgment to client
            if (g_state.g_client_pid > 0)
                kill(g_state.g_client_pid, SIGUSR1);
            
            // Display message and cleanup
            write(1, g_state.buffer, ft_strlen(g_state.buffer));
            free(g_state.buffer);
            g_state.buffer = NULL;
        }
        else
        {
            // Add character to buffer
            buffer_join(&g_state);
        }
        
        // Reset for next byte
        g_state.g_char = 0;
        g_state.g_bit_count = 0;
    }
}
```

### Client Byte Transmission

```c
void send_byte(int server_pid, unsigned char byte)
{
    int bit;
    
    bit = 7;  // Start from most significant bit
    while (bit >= 0)
    {
        // Send SIGUSR1 for bit 1, SIGUSR2 for bit 0
        if ((byte >> bit) & 1)
            kill(server_pid, SIGUSR1);
        else
            kill(server_pid, SIGUSR2);
        
        // Small delay to ensure signal is processed
        usleep(200);
        bit--;
    }
}
```

### Signal Initialization with sigaction

```c
static void init_signals(void)
{
    struct sigaction sa;
    
    ft_bzero(&sa, sizeof(sa));
    sa.sa_handler = handle_sigusr1;      // Handler function
    sigemptyset(&sa.sa_mask);            // Don't block other signals
    sa.sa_flags = 0;                     // No special flags
    sigaction(SIGUSR1, &sa, NULL);       // Register handler
}
```

### Dynamic Buffer Management

```c
void buffer_join(t_server_state *g_state)
{
    // Initialize buffer if empty
    if (!g_state->buffer)
        g_state->buffer = ft_strdup("");
    
    // Join buffer with new character
    g_state->new = ft_strjoin(g_state->buffer, &g_state->g_char);
    
    // Free old buffer
    if (g_state->buffer)
        free(g_state->buffer);
    
    if (!g_state->new)
        return;
    
    // Update buffer pointer
    g_state->buffer = g_state->new;
}
```

## 🔨 Compilation

### Using Makefile

```bash
make        # Compile both server and client
make clean  # Remove object files
make fclean # Remove executables and libraries
make re     # Recompile from scratch
```

### Manual Compilation

```bash
# Compile libraries
make -C Libft
make -C Printf

# Compile server
cc -Wall -Wextra -Werror server.c utilis.c -LLibft -lft -LPrintf -lftprintf -o server

# Compile client
cc -Wall -Wextra -Werror client.c -LLibft -lft -LPrintf -lftprintf -o client
```

### Output

Creates two executables:
- `server` - Server program
- `client` - Client program

## 💻 Usage

### Starting the Server

```bash
./server
```

Output:
```
Server PID: 12345
```

The server will wait indefinitely for messages.

### Sending Messages from Client

```bash
./client <server_pid> <message>
```

Example:
```bash
./client 12345 "Hello, World!"
```

Server output:
```
Hello, World!
```

Client output:
```
Message received by server!
```

### Complete Example Session

**Terminal 1 (Server):**
```bash
$ ./server
Server PID: 54321
Hello from client!
42 School is awesome!
```

**Terminal 2 (Client):**
```bash
$ ./client 54321 "Hello from client!"
Message received by server!

$ ./client 54321 "42 School is awesome!"
Message received by server!
```

### Multiple Clients

The server can handle multiple clients sequentially:

```bash
# Terminal 2
./client 54321 "Message 1"

# Terminal 3
./client 54321 "Message 2"

# Terminal 4
./client 54321 "Message 3"
```

## 🎁 Bonus Features

### 1. **Server Acknowledgment** ✅

The server sends a confirmation signal back to the client when the message is fully received:

**Client side:**
```c
void handle_sigusr1(int sig)
{
    ft_printf("Message received by server!\n");
}
```

**Server side:**
```c
if (g_state.g_client_pid > 0)
    kill(g_state.g_client_pid, SIGUSR1);
```

### 2. **Unicode Support** ✅

The implementation supports Unicode characters by transmitting them byte by byte:

```bash
./client 54321 "Hello 👋 World 🌍"
```

### 3. **Multiple Clients Support** ✅

The server can handle messages from different clients sequentially without mixing data.

### Bonus Score Breakdown

- **Base score:** 100/100
- **Acknowledgment system:** +10 points
- **Unicode support:** +10 points
- **Clean implementation:** +5 points
- **Total:** 125/100

## 🎯 Technical Challenges

### 1. **Signal Synchronization**

**Problem:** Signals can be lost if sent too quickly.

**Solution:** Add a small delay (`usleep(200)`) between each signal transmission.

### 2. **Bit Manipulation**

**Problem:** Converting characters to bits and reconstructing them correctly.

**Solution:** Use bit shifting and masking operations:
```c
// Extract bit: (byte >> bit) & 1
// Reconstruct: char = (char << 1) | bit
```

### 3. **Global Variable Management**

**Problem:** Signal handlers can't take parameters.

**Solution:** Use a global structure to maintain state between signals.

### 4. **Memory Management**

**Problem:** Dynamically growing buffer without leaks.

**Solution:** Careful allocation, reallocation, and freeing in `buffer_join()`.

### 5. **Client PID Tracking**

**Problem:** Identifying which client sent the message.

**Solution:** Use `siginfo_t` structure with `SA_SIGINFO` flag:
```c
sa.sa_sigaction = handle_sigusr;
sa.sa_flags = SA_SIGINFO;
```

### 6. **End of Message Detection**

**Problem:** Knowing when the message is complete.

**Solution:** Send a null terminator (`'\0'`) after the message.

## 🧪 Testing

### Basic Tests

```bash
# Test 1: Simple message
./client <pid> "Hello"

# Test 2: Long message
./client <pid> "This is a very long message to test the buffer management"

# Test 3: Special characters
./client <pid> "Test: !@#$%^&*()"

# Test 4: Numbers
./client <pid> "123456789"

# Test 5: Empty message
./client <pid> ""

# Test 6: Single character
./client <pid> "A"
```

### Advanced Tests

```bash
# Test 7: Very long message (stress test)
./client <pid> "$(python3 -c 'print("A" * 10000)')"

# Test 8: Unicode characters
./client <pid> "Hello 世界 🌍"

# Test 9: Multiple consecutive messages
for i in {1..10}; do ./client <pid> "Message $i"; done

# Test 10: Newlines in message
./client <pid> "Line 1
Line 2
Line 3"
```

### Error Handling Tests

```bash
# Test invalid PID
./client -1 "test"           # Error: Invalid server PID

# Test missing arguments
./client                      # Usage message

# Test wrong number of arguments
./client 123                  # Usage message

# Test non-existent server
./client 999999 "test"        # Signal fails (process doesn't exist)
```

### Valgrind Testing

```bash
# Check for memory leaks in server
valgrind --leak-check=full ./server

# In another terminal
./client <pid> "Test message"

# Stop server with Ctrl+C and check for leaks
```

## 📊 Performance Considerations

### Timing Parameters

| usleep Value | Speed | Reliability |
|--------------|-------|-------------|
| 100          | Fast  | May lose signals |
| 200          | Optimal | Stable |
| 500          | Slow  | Very stable |
| 1000         | Very Slow | Overkill |

### Transmission Speed

For `usleep(200)`:
- 1 character = 8 bits × 200μs = 1.6ms
- 100 characters ≈ 160ms
- 1000 characters ≈ 1.6s

## 🔍 Debugging Tips

### Server Debugging

```c
// Add debug output in signal handler
ft_printf("Received bit: %d (count: %d)\n", (sig == SIGUSR1), g_state.g_bit_count);
ft_printf("Current char: %c (%d)\n", g_state.g_char, g_state.g_char);
```

### Client Debugging

```c
// Add debug output in send_byte
ft_printf("Sending bit %d: %d\n", bit, (byte >> bit) & 1);
```

### Signal Flow Visualization

```bash
# Use strace to see system calls
strace -e signal ./server
strace -e signal ./client <pid> "test"
```

## 🎓 Learning Outcomes

This project teaches:

- **UNIX Signals** - Understanding and using `SIGUSR1` and `SIGUSR2`
- **Signal Handlers** - `signal()` vs `sigaction()`
- **Bit Manipulation** - Bitwise operations and binary representation
- **IPC** - Inter-process communication techniques
- **Asynchronous Programming** - Handling asynchronous events
- **Global Variables** - When and how to use them properly
- **Process Management** - PIDs, `kill()`, `pause()`

## 👨‍💻 Author

**rydelepi (Vibes33)** - 42 Student

*This project was validated with a score of 125/100, successfully implementing all mandatory and bonus requirements.*

---

## 📝 Notes

### Allowed Functions
- `write`, `ft_printf` and equivalents
- `signal`, `sigaction`, `sigemptyset`, `sigaddset`
- `kill`, `getpid`, `pause`
- `sleep`, `usleep`
- `malloc`, `free`, `exit`

### Forbidden Functions
- Any form of `printf` from standard library (must use `ft_printf`)
- Global variables (except for signal handlers)

### Norm Compliance
- Follows 42 Norm v3 strictly
- No global variables except in signal handler context
- Proper error handling and memory management

## 🚀 Possible Improvements

While this implementation meets all requirements, future enhancements could include:

- **Compression** - Compress message before transmission
- **Encryption** - Encrypt data for secure communication
- **Batch Transmission** - Send multiple bits per signal
- **Error Detection** - Add checksums or parity bits
- **Bidirectional Chat** - Full duplex communication
- **Multiple Servers** - Client can connect to different servers

---

*Part of the 42 School Common Core curriculum*
