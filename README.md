<img width="1000" height="231" alt="Animated GIF Upload Fit   Fill" src="https://github.com/user-attachments/assets/bde80a9a-3951-4321-8d80-d30b86a946aa" />

> **⚠️ Disclaimer & Security Notice**
> **2B-remote-agent** is an experimental educational and research project built for low-level systems programming practice. It includes keyboard event capture functionality (`WH_KEYBOARD_LL`).
> **Do not install or run this software on systems you do not own or administer, or without explicit authorization.** It lacks encryption, authentication, and security hardening, making it unsafe for production or public networks.

* * *

# 2B-remote-agent

An asynchronous remote system management and telemetry framework consisting of a WinAPI C client and a Python `asyncio` server.

## Overview

The framework provides low-level system event capture and telemetry ingestion over TCP connections.

```text
┌──────────────────────────┐          TCP          ┌──────────────────────────┐
│      Windows Client      │ ────────────────────> │      Python Server       │
│        (C / Win32)       │  Port 8080 (Default)  │        (asyncio)         │
│  • Keyboard event hook   │                       │  • Multi-client handler  │
│  • Layout conversion     │                       │  • File-based logging    │
└──────────────────────────┘                       └──────────────────────────┘

```

* * *

## System Components

### Windows Client (`C / Win32`)

- **Networking:** Winsock socket management targeting configurable IP/port (default `127.0.0.1:8080`).
- **Input Capture:** Low-level keyboard hook (`WH_KEYBOARD_LL`) with modifier and shortcut detection.
- **Encoding:** Unicode-to-UTF-8 character processing and layout resolution via Win32 API.
- **Metadata:** Transmits local system username on connection startup.

### Python Server (`asyncio`)

- **Concurrency:** Asynchronous TCP listener using `asyncio.start_server`.
- **Telemetry Ingestion:** Decodes incoming telemetry streams and records timestamps.
- **Logging:** Structured per-client file output managed via `aiofiles`.

### Telemetry Simulator

A standalone Python client (`run_simulator.py`) for server testing without requiring the C client runtime.

- **Supported Events:** Heartbeats, CPU/Memory/Disk/Network utilization, Process lifecycle, Service status, System health (temperature, battery), and error logging.

* * *

## Repository Layout

```text
2B-remote-agent/
├── client/
│   └── src/
│       ├── client.c
│       └── structure.h
├── server/
│   ├── src/
│   │   ├── tests/
│   │   │   └── client_simulator.py
│   │   ├── config_setup.py
│   │   ├── default_config.py
│   │   └── server.py
│   ├── requirements.txt
│   ├── run_server.py
│   └── run_simulator.py
└── README.md

```

* * *

## Getting Started

### Prerequisites

- **Client:** Windows OS, C compiler with Win32 and Winsock support (`ws2_32.lib`).
- **Server:** Python 3.8+ with `aiofiles`.

### 1\. Run Server

```bash
cd server
pip install -r requirements.txt
python run_server.py

```

*Prompt defaults to `127.0.0.1:8080` if left blank.*

### 2\. Run Telemetry Simulator (Optional)

```bash
cd server
python run_simulator.py

```

### 3\. Build & Run Client

Compile `client/src/client.c` using a Win32-compatible compiler (e.g., `gcc` via MinGW or MSVC `cl.exe` linking `ws2_32.lib`), then execute the binary:

```powershell
gcc client/src/client.c -o client.exe -lws2_32
.\client.exe

```

* * *

## Current Security Limitations

The current release is an early-stage research prototype. Known limitations include:

- **No Encryption:** Transmission is unencrypted plaintext (no TLS/SSL).
- **No Authentication:** Lacks identity verification or token exchange for client connections.
- **Unframed Protocols:** Lacks formal application-layer message framing and delivery verification.

* * *

## Development Roadmap

| Phase | Core Objective | Key Deliverables |
| --- | --- | --- |
| **Phase 1** | Networking & Telemetry | TLS implementation, structured packet framing, CI/CD pipeline |
| **Phase 2** | OS Internals & Infra | WDK kernel drivers, IOCTL integration, Terraform test environments |
| **Phase 3** | Sandboxing & Hardware | Embedded UART support, Docker sandboxing, system call tracing |

* * *

## Authors

- **Burak Yiğit Gerengi** — [burakyigitgerengi@gmail.com](https://www.google.com/search?q=mailto%3Aburakyigitgerengi%40gmail.com)
- **Berat Vargeloğlu** — [bv.sosyalmedya@gmail.com](https://www.google.com/search?q=mailto%3Abv.sosyalmedya%40gmail.com)

**GitHub Repository:** [2B-Systems/2B-remote-agent](https://github.com/2B-Systems/2B-remote-agent)
