# UDP Echo Server Sample

This sample demonstrates how to create a UDP echo server using the `network_system` library.

## Overview

The UDP echo server:
- **Listens** on a specified UDP port (default: 5555)
- **Receives** datagrams from clients
- **Echoes** received data back to the sender
- **Handles** multiple clients simultaneously (connectionless)
- **Provides** graceful shutdown on SIGINT/SIGTERM

## Key Features

- ✅ Connectionless UDP communication
- ✅ Endpoint-based routing (each datagram includes sender info)
- ✅ Asynchronous I/O with ASIO
- ✅ Thread-safe operations
- ✅ Error handling with callbacks
- ✅ Signal-based graceful shutdown

## Architecture

```
┌─────────────────────────────────────────────────────────┐
│                  UDP Echo Server                         │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  ┌──────────────┐        ┌─────────────┐               │
│  │  UDP Socket  │◄───────┤   Clients   │               │
│  │  Port 5555   │        │  (Multiple) │               │
│  └──────┬───────┘        └─────────────┘               │
│         │                                                │
│         ▼                                                │
│  ┌──────────────┐                                       │
│  │   Receive    │──► Process ──► Echo Back             │
│  │   Callback   │                                       │
│  └──────────────┘                                       │
│                                                          │
│  Features:                                               │
│  - No session management (stateless)                    │
│  - Direct datagram echo                                  │
│  - Concurrent client handling                            │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

## Building

```bash
# From samples directory
cmake -B build -DSAMPLES_USE_LOCAL_SYSTEMS=ON
cmake --build build --target udp_echo_server -j
```

## Usage

### Basic Usage

```bash
# Start server on default port (5555)
./build/bin/udp_echo_server

# Start server on custom port
./build/bin/udp_echo_server 6000
```

### Example Output

```
=================================================
  UDP Echo Server Sample
=================================================

[Server] Starting UDP echo server on port 5555...
[Server] UDP echo server is running on port 5555
[Server] Waiting for datagrams... (Press Ctrl+C to stop)

[Server] Received 18 bytes from 127.0.0.1:54321 - Message: "Hello, UDP Server!"
[Server] Sent echo response (24 bytes) to 127.0.0.1:54321
[Server] Received 18 bytes from 127.0.0.1:54322 - Message: "Testing UDP echo"
[Server] Sent echo response (24 bytes) to 127.0.0.1:54322

^C
[Server] Received signal 2, shutting down...
[Server] Stopping server...
[Server] Server stopped successfully.
```

## Code Example

```cpp
#include "network_system/core/messaging_udp_server.h"

using namespace network_system::core;

int main()
{
    // Create UDP server
    auto server = std::make_shared<messaging_udp_server>("UDPEchoServer");

    // Set up receive callback
    server->set_receive_callback(
        [server](const std::vector<uint8_t>& data,
                const asio::ip::udp::endpoint& sender)
        {
            std::string message(data.begin(), data.end());
            std::cout << "Received: " << message << "\n";

            // Echo back
            std::string echo_msg = "Echo: " + message;
            std::vector<uint8_t> echo_data(echo_msg.begin(), echo_msg.end());

            server->async_send_to(std::move(echo_data), sender,
                [](std::error_code ec, std::size_t bytes)
                {
                    if (!ec) {
                        std::cout << "Sent " << bytes << " bytes\n";
                    }
                });
        });

    // Start server
    auto result = server->start_server(5555);
    if (result.is_err()) {
        std::cerr << "Failed to start: " << result.error().message << "\n";
        return 1;
    }

    // Wait for shutdown signal
    server->wait_for_stop();

    return 0;
}
```

## API Reference

### messaging_udp_server

**Constructor:**
```cpp
messaging_udp_server(const std::string& server_id);
```

**Methods:**
- `start_server(uint16_t port)` - Start listening on specified port
- `stop_server()` - Stop the server gracefully
- `set_receive_callback(callback)` - Set datagram receive handler
- `set_error_callback(callback)` - Set error handler
- `async_send_to(data, endpoint, handler)` - Send datagram to specific endpoint
- `is_running()` - Check if server is running
- `wait_for_stop()` - Block until server stops

## Testing with netcat

You can test the server using `netcat`:

```bash
# In terminal 1: Start server
./build/bin/udp_echo_server

# In terminal 2: Send datagrams
echo "Hello UDP" | nc -u localhost 5555

# Or interactive mode
nc -u localhost 5555
```

## UDP vs TCP

**UDP Characteristics:**
- **Connectionless**: No handshake, datagrams sent directly
- **Stateless**: No sessions, each datagram independent
- **Unreliable**: No delivery guarantee, no retransmission
- **Fast**: Lower overhead, suitable for real-time applications
- **Order not guaranteed**: Datagrams may arrive out of order

**Use Cases:**
- Real-time gaming
- Video/audio streaming
- DNS queries
- IoT sensor data
- Broadcasting/multicasting

## Performance Notes

- **Throughput**: UDP has lower overhead than TCP
- **Latency**: Faster than TCP (no connection setup)
- **Packet Size**: Consider MTU (typically 1500 bytes for Ethernet)
- **Buffer Size**: Adjust based on expected datagram sizes

## Error Handling

The server provides error callbacks for:
- Socket creation failures
- Binding failures
- Receive errors
- Send errors

## Thread Safety

All public methods are thread-safe:
- Internal state protected by atomics
- Background ASIO thread handles I/O
- Callbacks invoked on ASIO worker thread

## See Also

- [udp_echo_client](../udp_echo_client/) - Corresponding UDP client
- [echo_server](../echo_server/) - TCP echo server for comparison
- [network_system Documentation](https://github.com/kcenon/network_system)
