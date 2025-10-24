# UDP Echo Client Sample

This sample demonstrates how to create a UDP client using the `network_system` library.

## Overview

The UDP echo client:
- **Connects** to a UDP server (default: localhost:5555)
- **Sends** test datagrams to the server
- **Receives** echo responses
- **Displays** statistics on completion
- **Handles** errors gracefully

## Key Features

- ✅ Connectionless UDP communication
- ✅ Asynchronous I/O with ASIO
- ✅ Thread-safe operations
- ✅ Automatic endpoint resolution
- ✅ Error handling with callbacks
- ✅ Statistics tracking (sent/received)

## Architecture

```
┌─────────────────────────────────────────────────────────┐
│                  UDP Echo Client                         │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  ┌──────────────┐        ┌─────────────┐               │
│  │  UDP Socket  │───────►│ UDP Server  │               │
│  │  (Dynamic)   │◄───────│   :5555     │               │
│  └──────┬───────┘        └─────────────┘               │
│         │                                                │
│         ▼                                                │
│  ┌──────────────┐                                       │
│  │   Receive    │──► Process Response                   │
│  │   Callback   │                                       │
│  └──────────────┘                                       │
│                                                          │
│  Flow:                                                   │
│  1. Resolve target endpoint                             │
│  2. Send datagrams with send_packet()                   │
│  3. Receive responses via callback                       │
│  4. Display statistics                                   │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

## Building

```bash
# From samples directory
cmake -B build -DSAMPLES_USE_LOCAL_SYSTEMS=ON
cmake --build build --target udp_echo_client -j
```

## Usage

### Basic Usage

```bash
# Connect to localhost:5555 (default)
./build/bin/udp_echo_client

# Connect to specific host and port
./build/bin/udp_echo_client example.com 6000
```

### Example Output

```
=================================================
  UDP Echo Client Sample
=================================================

[Client] Connecting to UDP server at localhost:5555...
[Client] Connected to localhost:5555

[Client] Sending 5 test messages...

[Client] Sending: "Hello, UDP Server!"
[Client] Sent "Hello, UDP Server!" (18 bytes)
[Client] Received response: "Echo: Hello, UDP Server!"
[Client] From: 127.0.0.1:5555

[Client] Sending: "This is message #2"
[Client] Sent "This is message #2" (18 bytes)
[Client] Received response: "Echo: This is message #2"
[Client] From: 127.0.0.1:5555

...

[Client] Waiting for responses...

=================================================
  Results
=================================================
Messages Sent:      5
Responses Received: 5
=================================================

[Client] Stopping client...
[Client] Client stopped successfully.
```

## Code Example

```cpp
#include "network_system/core/messaging_udp_client.h"

using namespace network_system::core;

int main()
{
    // Create UDP client
    auto client = std::make_shared<messaging_udp_client>("UDPEchoClient");

    // Set up receive callback
    client->set_receive_callback(
        [](const std::vector<uint8_t>& data,
          const asio::ip::udp::endpoint& sender)
        {
            std::string message(data.begin(), data.end());
            std::cout << "Received: " << message << "\n";
        });

    // Start client
    auto result = client->start_client("localhost", 5555);
    if (result.is_err()) {
        std::cerr << "Failed to start: " << result.error().message << "\n";
        return 1;
    }

    // Send datagram
    std::string msg = "Hello, UDP Server!";
    std::vector<uint8_t> data(msg.begin(), msg.end());

    client->send_packet(std::move(data),
        [](std::error_code ec, std::size_t bytes)
        {
            if (!ec) {
                std::cout << "Sent " << bytes << " bytes\n";
            }
        });

    // Wait for response
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Stop client
    client->stop_client();

    return 0;
}
```

## API Reference

### messaging_udp_client

**Constructor:**
```cpp
messaging_udp_client(std::string_view client_id);
```

**Methods:**
- `start_client(host, port)` - Start client and resolve target endpoint
- `stop_client()` - Stop the client gracefully
- `send_packet(data, handler)` - Send datagram to target endpoint
- `set_receive_callback(callback)` - Set datagram receive handler
- `set_error_callback(callback)` - Set error handler
- `set_target(host, port)` - Change target endpoint
- `is_running()` - Check if client is running
- `wait_for_stop()` - Block until client stops

## Testing with Server

```bash
# Terminal 1: Start server
./build/bin/udp_echo_server

# Terminal 2: Run client
./build/bin/udp_echo_client
```

## UDP vs TCP Client

**UDP Client Characteristics:**
- **No connection setup**: Start sending immediately
- **No connection state**: Each send is independent
- **Lightweight**: Minimal overhead
- **Fast startup**: No three-way handshake
- **No delivery guarantee**: Application must handle reliability

**When to use UDP Client:**
- Real-time applications (gaming, VoIP)
- Broadcast/multicast scenarios
- Time-sensitive data (sensor readings)
- DNS lookups
- DHCP requests

## Performance Notes

- **Throughput**: Lower per-packet overhead than TCP
- **Latency**: Minimal latency (no connection setup)
- **Packet Loss**: Application must handle packet loss
- **Order**: Datagrams may arrive out of order
- **MTU**: Consider Path MTU (typically 1500 bytes)

## Error Handling

The client provides error callbacks for:
- Endpoint resolution failures
- Socket creation failures
- Send errors
- Receive errors

Example error handling:

```cpp
client->set_error_callback(
    [](std::error_code ec)
    {
        std::cerr << "Error: " << ec.message() << "\n";
    });
```

## Thread Safety

All public methods are thread-safe:
- Socket access protected by mutexes
- Atomic flags prevent race conditions
- send_packet() can be called from any thread

## Advanced Usage

### Changing Target Endpoint

```cpp
// Change target during runtime
client->set_target("newserver.com", 6000);
```

### Custom Message Format

```cpp
// Send binary data
struct Message {
    uint32_t id;
    uint64_t timestamp;
    char data[100];
};

Message msg{1, time(nullptr), "test"};
std::vector<uint8_t> data(
    reinterpret_cast<uint8_t*>(&msg),
    reinterpret_cast<uint8_t*>(&msg) + sizeof(msg)
);

client->send_packet(std::move(data), handler);
```

## See Also

- [udp_echo_server](../udp_echo_server/) - Corresponding UDP server
- [echo_client](../echo_client/) - TCP echo client for comparison
- [network_system Documentation](https://github.com/kcenon/network_system)
