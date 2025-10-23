# Echo Client

## Overview

This sample demonstrates the network_system client features including:

- TCP client implementation using ASIO
- Connection to echo server
- Message sending with container protocol
- Response receiving and validation
- Connection management
- Error handling

## Building

```bash
cd /path/to/samples
mkdir -p build && cd build
cmake -- -DSAMPLES_USE_LOCAL_SYSTEMS=ON
make echo_client
```

## Running

```bash
cd build
./bin/echo_client
```

**Note**: The echo_server must be running before starting the client.

## Expected Output

```
Echo client starting...
Creating logger...
Connecting to server at 127.0.0.1:29000...
Connected to server

Sending message 0...
[INFO] Sent: Test message 0
[INFO] Received echo: Test message 0

Sending message 1...
[INFO] Sent: Test message 1
[INFO] Received echo: Test message 1

...

All messages sent and received successfully
Disconnecting...
Client stopped
```

## Features Demonstrated

### 1. TCP Client Connection

```cpp
auto client = std::make_shared<messaging_client>(
    io_context,
    "127.0.0.1",  // Server host
    29000,        // Server port
    logger
);

client->connect();
```

### 2. Message Creation and Sending

```cpp
// Create message container
value_container message;
message.add(std::make_shared<value>(
    "command", value_types::string_value, "echo"));
message.add(std::make_shared<value>(
    "payload", value_types::string_value, "Test message"));

// Send message
client->send(std::move(message));
```

### 3. Response Handling

```cpp
// Receive echo response
auto response = client->receive();

if (response) {
    auto payload = response->get_value("payload");
    std::cout << "Received: " << payload->data() << std::endl;
}
```

## Testing Setup

### 1. Start Server

```bash
# Terminal 1
cd build
./bin/echo_server
```

### 2. Run Client

```bash
# Terminal 2
cd build
./bin/echo_client
```

## Key Concepts

- **Client-Server Model**: TCP-based request-response pattern
- **Message Protocol**: Container-based structured messages
- **Async I/O**: Non-blocking network operations
- **Connection Management**: Connect, send, receive, disconnect lifecycle

## Connection Parameters

- **Host**: 127.0.0.1 (localhost)
- **Port**: 29000
- **Protocol**: TCP

## Related Samples

- [Echo Server](../echo_server/README.md) - Companion server application
- [Container Sample](../container_sample/README.md) - Message container format
- [Logging Sample](../logging_sample/README.md) - Logger usage
