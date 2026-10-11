> **⚠️ Disclaimer & Security Notice**
> **2B-remote-agent** is an experimental educational and research project built for low-level systems programming practice. It captures keyboard events (`WH_KEYBOARD_LL`) and collects identifying system information.
> **Do not install or run this software on systems you do not own or administer, or without explicit authorization.** It lacks encryption, authentication, and security hardening, making it unsafe for production or public networks. Captured input and system information are stored in plaintext logs.

---

<img width="1000" height="231" alt="2B-remote-agent project banner" src="https://github.com/user-attachments/assets/bde80a9a-3951-4321-8d80-d30b86a946aa" />

# 2B-remote-agent

An experimental remote system management and telemetry framework consisting of a modular WinAPI C client and a Python `asyncio` server.

## Overview

The Windows client connects to the server over TCP (default `127.0.0.1:8080`), sends a system-information snapshot, then streams keyboard events. The server accepts multiple clients and saves timestamped records to a separate log for each connection.

**Current scope:** the Windows client sends system information and keyboard events. CPU, memory, disk, network, process, and service events are synthetic simulator data. Remote command execution is not implemented.

## System Components

### Windows Client (`C / Win32`)

- **Modular structure:** `main.c` manages startup and cleanup; `utils.c` handles configuration, networking, encoding, and JSON transmission; `keyboard.c` handles input capture; `os.c` gathers system information; `info-sender.c` assembles the startup snapshot.
- **Networking:** Winsock TCP over IPv4. Press Enter for `127.0.0.1:8080`, or enter `custom` to supply an IPv4 address and a port from `1` to `65535`.
- **Input capture:** low-level keyboard hook (`WH_KEYBOARD_LL`) with modifier, shortcut, AltGr, special-key, and repeated-key handling.
- **Encoding:** foreground keyboard-layout resolution via `ToUnicodeEx`, with wide-character strings converted to UTF-8.
- **Serialization:** bundled `cJSON` produces compact JSON objects, each terminated by a newline.
- **System information:** sends the following fields once after connecting and installing the keyboard hook:

| JSON field | Content |
| --- | --- |
| `Username` | Current Windows username |
| `Hostname/FQDN` | Fully qualified DNS computer name |
| `OS` | Windows version label and build number |
| `OS Architecture` | Native system architecture |
| `Uptime` | Days, hours, minutes, and seconds since boot |
| `Boot Time` | Estimated local boot time in `DD.MM.YYYY HH:MM:SS` format |

The snapshot is not refreshed periodically. Information lookup failures may produce fallback strings such as `Unknown`.

### Python Server (`asyncio`)

- **Concurrency:** asynchronous TCP listener using `asyncio.start_server`.
- **Configuration:** interactive IPv4/IPv6 address and port validation, defaulting to `127.0.0.1:8080`. The Windows client currently supports IPv4 only.
- **Ingestion:** reads newline-delimited UTF-8 records and prefixes each with a server-local ISO 8601 timestamp.
- **Logging:** appends records using `aiofiles` to connection-specific files under `server/logs/`, creating the directory on startup.
- **Lifecycle:** reads multiple records per connection, closes each client when its handler exits, and closes the listener during shutdown.

The server stores records as text; it does not parse JSON, validate telemetry fields, or authenticate the reported identity.

### Telemetry Simulator

`server/run_simulator.py` tests ingestion without running the Windows client or capturing real keyboard input.

- Sends `Username: SIMULATOR` once, followed by randomly selected newline-delimited text records on one persistent connection.
- Generates synthetic heartbeats, resource usage, process/service events, temperature/battery data, login/file events, warnings, errors, and update notices.
- Includes Unicode, blank, malformed, and other edge-case records.
- Prompts for the server address, port, and message delay. Delay defaults to a random **1–5 seconds**; select `n` to send without that delay.

Simulator records remain plain text rather than the Windows client's JSON format.

## Record Format

Each Windows client record is a compact UTF-8 JSON object followed by `\n`. Example records (illustrative values):

```json
{"Username":"test-user","Hostname/FQDN":"lab-pc.example.local","OS":"Windows 11 (Build 22631)","OS Architecture":"64-bit (x64)","Uptime":"0 Days, 2 Hours, 15 Minutes, 0 Seconds","Boot Time":"11.10.2026 16:00:00"}
{"Key":"a"}
{"Key":"[CTRL+C]"}
```

A stored log entry looks like:

```text
[2026-10-11T18:15:00] {"Key":"[CTRL+C]"}
```

Log files are named `<sanitized-client-IP>_<source-port>.log`. A new connection with a different source port uses a different file. Runtime logs are ignored by Git.

## Repository Layout

| Path | Purpose |
| --- | --- |
| `client/src/main.c` | Client entry point |
| `client/src/include.h` | Shared Windows, standard-library, and project includes |
| `client/src/utils.c`, `utils.h` | Configuration, sockets, UTF-8 conversion, JSON helpers, message loop |
| `client/src/keyboard.c`, `keyboard.h` | Keyboard hook and event processing |
| `client/src/os.c`, `os.h` | System-information collection |
| `client/src/info-sender.c`, `info-sender.h` | Startup system-information JSON |
| `client/src/cJSON.c`, `cJSON.h` | Bundled JSON library |
| `server/run_server.py` | Interactive server entry point |
| `server/run_simulator.py` | Interactive simulator entry point and sample records |
| `server/src/server.py` | TCP listener and client handlers |
| `server/src/server_client_connection.py` | Record reception and log writing |
| `server/src/server_config.py` | Address and port validation |
| `server/src/tests/client_simulator.py` | Simulator connection and message logic |
| `server/requirements.txt` | Python dependency (`aiofiles`) |
| `server/logs/` | Generated connection logs |
| `README.md` | English documentation |

## Getting Started

### Prerequisites

- **Server/simulator:** Python **3.10+** and `aiofiles`.
- **Windows client:** Windows, a C compiler, and the Windows SDK. The code uses MSVC-style secure CRT functions; the build command below targets MSVC.
- **Client libraries:** Winsock (`ws2_32.lib`), keyboard/window APIs (`user32.lib`), and username lookup (`advapi32.lib`). `cJSON` is included in the repository.

### 1. Run the Server

From the repository root:

```bash
cd server
python -m pip install -r requirements.txt
python run_server.py
```

Press Enter at both prompts to listen on `127.0.0.1:8080`. Received records appear in the console and in `server/logs/`.

### 2. Run the Telemetry Simulator (Optional)

In another terminal, from the repository root:

```bash
cd server
python run_simulator.py
```

Use the same server address and port. Press Enter at the delay prompt for random 1–5 second intervals. Stop the simulator with Ctrl+C.

### 3. Build and Run the Windows Client

Open an **MSVC Developer Command Prompt** at the repository root and compile all six C source files:

```bat
cl /nologo /W4 /TC client\src\main.c client\src\utils.c client\src\keyboard.c client\src\os.c client\src\info-sender.c client\src\cJSON.c /Fe:client.exe /link ws2_32.lib user32.lib advapi32.lib
.\client.exe
```

Press Enter to connect to `127.0.0.1:8080`, or type `custom` for another IPv4 endpoint. Run only in an authorized test environment; keyboard capture starts after the connection succeeds.

The previous single-file `client/src/client.c` build command no longer applies. Alternative toolchains may require compatibility changes for the secure CRT calls.

## Current Security and Reliability Limitations

This is an early-stage research prototype:

- **No encryption:** records travel over plaintext TCP and are stored in plaintext.
- **No authentication:** system-information fields are self-reported and do not verify client identity.
- **Minimal framing and validation:** newline-delimited JSON is used by the Windows client, but the server accepts arbitrary UTF-8 text without schema validation, application-level integrity checks, or acknowledgments.
- **No delivery recovery:** client sends do not retry partial writes; there is no reconnect logic, buffering for later delivery, or application-level delivery verification.
- **Blocking client I/O:** socket transmission happens synchronously, including within the keyboard callback.

## Development Roadmap

These are planned objectives, not implemented capabilities.

| Phase | Core Objective | Key Deliverables |
| --- | --- | --- |
| **Phase 1** | Networking & Telemetry | TLS implementation, structured packet framing, CI/CD pipeline |
| **Phase 2** | OS Internals & Infra | WDK kernel drivers, IOCTL integration, Terraform test environments |
| **Phase 3** | Sandboxing & Hardware | Embedded UART support, Docker sandboxing, system call tracing |

## Authors

- **Burak Yiğit Gerengi** — [burakyigitgerengi@gmail.com](mailto:burakyigitgerengi@gmail.com)
- **Berat Vargeloğlu** — [bv.sosyalmedya@gmail.com](mailto:bv.sosyalmedya@gmail.com)

**GitHub Repository:** [2B-Systems/2B-remote-agent](https://github.com/2B-Systems/2B-remote-agent)

