# Combined Integration Sample

This sample demonstrates how to integrate multiple systems (Logger, Container, and Threads) in a real-world application scenario.

## Overview

The combined sample shows:
- **Async Logging** - Using logger with thread pool for asynchronous operations
- **Data Storage** - Storing processing results in containers
- **Concurrent Processing** - Processing multiple jobs using thread pool
- **System Integration** - Combining multiple systems for real-world scenarios

## Architecture

```
┌─────────────────────────────────────────────────────────┐
│                  Combined Sample                         │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  ┌──────────┐        ┌──────────┐       ┌───────────┐  │
│  │  Logger  │◄───────┤  Thread  │──────►│ Container │  │
│  │  System  │        │   Pool   │       │  System   │  │
│  └──────────┘        └──────────┘       └───────────┘  │
│       │                    │                   │        │
│       │                    │                   │        │
│       ▼                    ▼                   ▼        │
│  [Async Logs]         [10 Workers]        [Results]    │
│                                                          │
└─────────────────────────────────────────────────────────┘
