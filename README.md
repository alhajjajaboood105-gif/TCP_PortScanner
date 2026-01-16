# TCP Port Scanner

**Author:**[@alhajjajabood](https://github.com/alhajjajabood),[@khamees](https://github.com/khamees06) ,[@zenaqdh](https://github.com/zenaqdh),[@hebaghass283-art](https://github.com/hebaghass283-art)  
**Date:** December 2025  
**Language:** C++11  
**Platform:** Windows, Linux  

---

## 📋 Overview

A cross‑platform, multi‑threaded TCP port scanner with a console‑based interactive menu. Scans IP ranges, detects common services, and provides real‑time progress feedback.

## 🛠️ Features

- **Cross‑Platform** – Works on Windows (Winsock) and Linux (POSIX sockets)
- **Multi‑Threaded** – Configurable thread count (1‑200) for parallel scanning
- **Service Detection** – Identifies 14 common services (FTP, SSH, HTTP, etc.)
- **Progress Display** – Real‑time progress bar and statistics
- **Interactive Menu** – Easy configuration via console interface
- **Graceful Shutdown** – Clean interrupt handling (Ctrl+C)
- **Open Development** – Free to modify, upgrade, and extend

## 📦 Compilation

### Linux
```bash
g++ -std=c++11 -pthread tcpportscaner.cpp -o tcpportscaner
```

### Windows (MinGW)
```bash
g++ -std=c++11 tcpportscaner.cpp -o tcpportscaner.exe -lws2_32
```

## 🚀 Usage

```bash
./tcpportscaner   # Linux
tcpportscaner.exe # Windows
```

### Menu Options
1. Configure Target IP
2. Configure Port Range (1‑65535)
3. Configure Threads (1‑200)
4. Configure Timeout (100‑5000 ms)
5. Quick Scan (Ports 1‑1024)
6. Common Services Scan (21, 22, 80, 443, etc.)
7. Run Scan
8. View Current Configuration
9. Help
0. Exit

## 🔧 Development & Contribution

This project is open for updates and upgrades. Feel free to:
- Add new scanning techniques (UDP, SYN scan, etc.)
- Implement additional service detection
- Improve the user interface
- Optimize performance
- Fix bugs and enhance platform compatibility

## ⚠️ Legal & Security Notice

**Only scan systems you own or have explicit written permission to test.**  
Unauthorized port scanning is illegal in many jurisdictions.

✅ **Permitted:** Your own machines, authorized test environments  
❌ **Prohibited:** Public networks, systems you don't own, malicious intent

## 📄 License

Educational use only. Use responsibly and ethically.  
**Open Source:** Free to use, modify, and distribute with attribution.


