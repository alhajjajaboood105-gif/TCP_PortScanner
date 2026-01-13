TCP PORT SCANNER 

Author: Abood Alhajjaj , Khamees Alhalboosi , Zena Alqudah ,Heba Alhasasneh\
Date: December 2025\
Language: C++11\
Platforms: Windows, Linux

---

 Table of Contents

1. [Project Overview](project-overview)
2. [Library Dependencies & Usage](library-dependencies--usage)
3. [Architecture & Design Patterns](architecture--design-patterns)
4. [Platform Abstractions Layer](platform-abstractions-layer)
5. [Core Components Explained](core-components-explained)
6. [Configuration Constants](configuration-constants)
7. [Usage Guide](usage-guide)
8. [Threading Model](threading-model)
9. [Error Handling Strategy](error-handling-strategy)
10. [Performance Optimization](performance-optimization)
11. [Service Identification](service-identification)
12. [Legal & Security Considerations](legal--security-considerations)

---

 Project Overview

Purpose
A cross-platform TCP port scanner that identifies open ports on target systems using multi-threaded concurrent connection attempts.

Key Capabilities
- Multi-threaded Scanning: 1-200 concurrent threads for parallel port checking
- Cross-Platform: Works on Windows and Linux without code changes
- Service Detection: Identifies common services by port number
- Progress Tracking: Real-time visual feedback during scanning
- Interactive Menu: User-friendly configuration interface
- Graceful Shutdown: Handles Ctrl+C interruption cleanly

Technical Specifications
- Language: C++11
- Compilation: Standard g++/clang++
- Dependencies: Standard C++ Library + Platform-specific socket APIs
- Architecture: Object-oriented with abstract interfaces

---

 Library Dependencies & Usage

Standard C++ Headers

 #include <iostream>
Purpose: Console Input/Output Operations\
Why Needed: Display prompts, results, progress, and error messages\
Usage in Code:

cout << "\nStarting scan of " << targetip << "...\n";   Output
cout << "Press Enter To Continue...";                   Input prompt
cerr << "WSAStartup failed\n";                          Error output


Key Functions:
- cout - Standard output stream
- cin - Standard input stream  
- cerr - Error output stream
- flush - Force output to display

---

 #include <string>
Purpose: String Storage and Manipulation\
Why Needed: Store and process IP addresses, port names, user input\
Usage in Code:

string targetip = "192.168.1.1";           Store IP
string service = ideserv(80);              Get service name
string input = stringutils::toLower(str);  Convert to lowercase


Key Functions:
- .c_str() - Convert to C-style string
- .empty() - Check if empty
- .find() / .substr() - String searching
- .find_first_not_of() - Find first non-matching character

---

 #include <vector>
Purpose: Dynamic Array Container\
Why Needed: Store variable-size collections (open ports, threads)\
Usage in Code:

vector<int> openports;               Store discovered open ports
vector<thread> workerthreads;        Store worker thread objects
vector<int> commonport = {21,22,80,443};   Initialize with values
openports.push_back(port);           Add element
openports.clear();                   Clear all elements


Key Functions:
- .push_back() - Add element to end
- .emplace_back() - Construct element in place
- .size() - Get number of elements
- .empty() - Check if empty
- .clear() - Remove all elements
- operator[] - Access by index

---

 #include <thread>
Purpose: Multi-Threading Support\
Why Needed: Create worker threads for parallel port scanning\
Parameters Explained:

 Create thread that runs workersf with parameter i
workerthreads.emplace_back(&portscanner::workersf, this, i);

 Wait for thread completion
for (auto &thread : workerthreads) {
    if (thread.joinable()) 
        thread.join();   Block until thread finishes
}


Key Functions:
- std::thread(callable, args...) - Create thread
- .join() - Wait for thread completion
- .joinable() - Check if join() can be called
- .detach() - Detach thread (not used here)

Thread Execution Model:
- Each thread runs workersf(threadIndex) independently
- Threads execute in parallel on multi-core systems
- Main thread continues until all workers .join()

---

 #include <mutex>
Purpose: Thread Synchronization & Mutual Exclusion\
Why Needed: Prevent race conditions when multiple threads access shared data\
Parameters Explained:

mutex resltsmutex;   Declare mutex

 Lock-based access (Automatic RAII pattern)
{
    lock_guard<mutex> lock(resltsmutex);
     Only one thread can execute this block
    openports.push_back(port);   Thread-safe access
     Lock automatically released when lock_guard destroyed
}


Key Functions:
- std::mutex - Mutex object
- std::lock_guard<std::mutex> - RAII lock (auto-releases)
- .lock() / .unlock() - Manual lock/unlock
- .try_lock() - Non-blocking attempt

Race Condition Prevention:

 UNSAFE (without mutex):
openports.push_back(port);   Thread 1
openports.push_back(port);   Thread 2 - Possible crash!

 SAFE (with mutex):
{
    lock_guard<mutex> lock(resltsmutex);
    openports.push_back(port);   Only one thread at a time
}


---

 #include <cstring>
Purpose: C-Style String & Memory Operations\
Why Needed: Zero out socket structures, copy memory blocks\
Usage in Code:

struct sockaddr_in target_addr;
memset(&target_addr, 0, sizeof(target_addr));
 Zeros all bytes in structure before use


Key Functions:
- memset(ptr, value, size) - Set all bytes to value
- memcpy(dest, src, size) - Copy memory
- strlen() - Get string length

---

 #include <cstdlib>
Purpose: General Utilities & Allocation\
Why Needed: Dynamic memory allocation for strategy objects\
Usage in Code:

scanstratgy = new tcpstratgy();    Allocate
delete scanstratgy;                Deallocate


Key Functions:
- new / delete - Dynamic allocation
- stoi() - String to integer conversion
- exit() - Exit program

---

 #include <cctype>
Purpose: Character Classification\
Why Needed: Validate numeric input, check character types\
Usage in Code:

bool isnumber(const string &str) {
    for (char c : str) {
        if (!isdigit(c))   Check if digit (0-9)
            return false;
    }
    return true;
}


Key Functions:
- isdigit(c) - Check if 0-9
- isalpha(c) - Check if letter
- isspace(c) - Check if whitespace
- tolower(c) / toupper(c) - Case conversion

---

 #include <algorithm>
Purpose: Standard C++ Algorithms\
Why Needed: String transformation, sorting results\
Usage in Code:

 Convert string to lowercase
transform(result.begin(), result.end(), result.begin(), ::tolower);

 Sort open ports for display
sort(sortedport.begin(), sortedport.end());

 Get minimum value
min(threadindex, extraports);


Key Functions:
- transform() - Apply function to range
- sort() - Sort range
- min() / max() - Minimum/maximum values

---

 #include <iomanip>
Purpose: Input/Output Formatting\
Why Needed: Format columns, percentages, and progress display\
Parameters Explained:

 Set column width
cout << setw(5) << port << "\n";   Right-align in 5 chars

 Set decimal precision
cout << fixed << setprecision(1) << 45.2 << "%";
 Output: 45.2%

 Left alignment
cout << left << setw(10) << "PORT" << "SERVICE\n";


Key Manipulators:
- setw(n) - Set field width
- setprecision(n) - Set decimal places
- fixed - Fixed-point notation
- left / right - Alignment

---

 #include <csignal>
Purpose: Signal Handling (Ctrl+C)\
Why Needed: Graceful shutdown when user interrupts\
Usage in Code:

signal(SIGINT, handleSignal);    Register Ctrl+C handler
signal(SIGTERM, handleSignal);   Register termination handler

static void handleSignal(int) {
    interrupted = true;
    cout << "\n\nScan Interrupted By User\n";
}


Key Signals:
- SIGINT - Interrupt signal (Ctrl+C)
- SIGTERM - Termination signal
- SIGKILL - Kill signal (cannot be caught)

---

 #include <atomic>
Purpose: Atomic Operations for Thread-Safe Flags\
Why Needed: Lock-free flag signaling to stop threads\
Usage in Code:

atomic<bool> scanning(false);

 In worker threads - check without locking
if (!scanning) break;   Atomic read - very fast

 In main thread - set flag
scanning = false;   Atomic write - signals all threads


Why Atomic? Faster than mutex for simple flags; no lock overhead

Parameters:
- atomic<bool> - Atomic boolean type
- No locks needed for read/write operations
- Guaranteed memory visibility across threads

---

 #include <chrono>
Purpose: Time & Duration Operations\
Why Needed: Implement timeout mechanism for socket operations\
Usage in Code:

 Sleep for milliseconds
this_thread::sleep_for(chrono::milliseconds(ms));

 Convert milliseconds to timeval for select()
timeout.tv_sec = timeoutms / 1000;
timeout.tv_usec = (timeoutms % 1000) * 1000;


Key Components:
- chrono::milliseconds(n) - Duration in milliseconds
- chrono::seconds(n) - Duration in seconds
- this_thread::sleep_for() - Sleep duration

---

Platform-Specific Headers

 Windows-Only: #include <winsock2.h>
When Used: Compiled with #ifdef _WIN32\
Purpose: Windows Socket API\
Key Components:

| Function | Purpose | Parameters |
|----------|---------|------------|
| WSAStartup() | Initialize Winsock | Version, WSA data structure |
| WSACleanup() | Cleanup Winsock | None |
| socket() | Create socket | Family (AF_INET), Type (SOCK_STREAM), Protocol (0) |
| connect() | Connect to host | Socket, address, address length |
| closesocket() | Close socket | Socket descriptor |
| WSAGetLastError() | Get error code | None |

Usage:

#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);   Initialize
    
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        int error = WSAGetLastError();
         Handle error
    }
    closesocket(sockfd);
    WSACleanup();   Cleanup
#endif


Windows-Specific Functions Used:

 Windows: Non-blocking mode
u_long mode = 1;
ioctlsocket(s, FIONBIO, &mode);

 Windows: IP address parsing
inetPtonA(AF_INET, ip.c_str(), &out)

 Windows: Error checking
int e = WSAGetLastError();
if (e == WSAEWOULDBLOCK || e == WSAEINPROGRESS || e == WSAEALREADY)


---

 Linux-Only Headers
When Used: Compiled on Linux/Unix (not #ifdef _WIN32)

| Header | Functions | Purpose |
|--------|-----------|---------|
| <sys/socket.h> | socket(), connect() | Socket creation/operations |
| <netinet/in.h> | sockaddr_in, htons() | Network structures |
| <arpa/inet.h> | inet_pton() | IP address conversion |
| <unistd.h> | close(), geteuid() | POSIX system calls |
| <fcntl.h> | fcntl() | File control operations |
| <sys/select.h> | select(), fd_set | I/O multiplexing |
| <errno.h> | errno | Error reporting |

Linux-Specific Functions Used:

 Linux: Non-blocking mode
int flags = fcntl(s, F_GETFL, 0);
fcntl(s, F_SETFL, flags | O_NONBLOCK);

 Linux: IP address parsing
inet_pton(AF_INET, ip.c_str(), &out)

 Linux: Error checking
if (errno == EINPROGRESS)

 Linux: Check if running as root
if (geteuid() == 0)   Root UID is 0

 Linux: Close socket
close(sockfd);


---

 Architecture & Design Patterns

1. Abstract Interface Pattern

 scannerabs Interface

class scannerabs {
public:
    virtual void scan() = 0;            Start scanning
    virtual void stop() = 0;            Stop and cleanup
    virtual vector<int> getResults() const = 0;   Get open ports
    virtual ~scannerabs() {}            Virtual destructor
};

Purpose: Define contract for scanner implementations\
Benefits:
- Loose coupling between scanner and manager
- Easy to create alternative scanner implementations
- Polymorphic behavior

---

 stratgyabs Interface

class stratgyabs {
public:
    virtual bool checkport(const string& ip, int port, int timeout_ms) = 0;
    virtual ~stratgyabs() {}
};

Purpose: Encapsulate port-checking algorithms\
Parameters:
- ip - Target IP address (e.g., "192.168.1.1")
- port - Port number to check (1-65535)
- timeout_ms - Wait timeout in milliseconds (100-5000)
- Returns: true if port is open (connection accepted), false if closed or timeout

Strategy Pattern Benefits:
- Allows different scanning methods without modifying scanner
- Can swap TCP, UDP, or other strategies at runtime
- Encapsulates algorithm variations

---

2. Concrete Implementations

 tcpstratgy Class
Method: connectwithtimeout(sockefd, addr, timeoutms)

Step-by-Step Process:

1. Set Non-Blocking Mode

if (!set_nonblocking(sockefd)) return false;
 Makes socket not wait for completion


2. Initiate Connection

int result = connect(sockefd, (struct sockaddr*)&addr, sizeof(addr));

- Returns immediately (non-blocking)
- Returns 0 if connected instantly (rare for TCP)
- Returns error code if not yet connected (expected)

3. Check for Expected "In Progress" Error

#ifdef _WIN32
    if (result == 0) return true;   Connected immediately
    if (!isprograes_error()) return false;   Real error
#else
    if (result == 0) return true;
    if (errno != EINPROGRESS) return false;   Real error
#endif


4. Setup File Descriptor Set for select()

fd_set writefds;            Create set
FD_ZERO(&writefds);         Initialize empty
FD_SET((unsigned int)sockefd, &writefds);  Add socket to set
 writefds now contains only our socket


5. Setup Timeout Structure

struct timeval timeout;
timeout.tv_sec = timeoutms / 1000;            Seconds
timeout.tv_usec = (timeoutms % 1000) * 1000;  Microseconds
 Example: 500ms = 0 seconds + 500000 microseconds


6. Wait for Connection with Timeout

result = select((int)sockefd + 1,   Highest fd + 1
                NULL,                Read fds (not used)
                &writefds,           Write fds (check socket ready)
                NULL,                Exception fds (not used)
                &timeout);           Timeout
 Returns:
 > 0: Socket is writable (connection completed or error)
 0: Timeout expired
 < 0: Error in select()


7. Verify Connection Succeeded

int error = 0;
socklen_t len = sizeof(error);
getsockopt(sockefd, SOL_SOCKET, SO_ERROR, (char*)&error, &len);

#ifdef _WIN32
    return error == 0 || error == WSAEWOULDBLOCK;
#else
    return error == 0;   True if no error = port is open
#endif


---

 portscanner Class (Implements scannerabs)
Responsibilities:
1. Divide port range among threads
2. Create and manage worker threads
3. Collect results from all threads
4. Display progress and final results
5. Identify services by port number

Key Algorithm: Port Range Division

int totalports = endport - startport + 1;       Total ports to scan
int portsPerthread = totalports / threadsn;     Base ports per thread
int extraports = totalports % threadsn;         Remaining ports

 Example: 1000 ports, 3 threads
 portsPerthread = 333
 extraports = 1

 Thread 0: ports 0-334 (333 + 1 extra)
 Thread 1: ports 335-667
 Thread 2: ports 668-999

 Calculation for thread i:
int offset = threadindex * portsPerthread + min(threadindex, extraports);
int threadstart = startport + offset;
int threadend = threadstart + portsPerthread - 1;
if (threadindex < extraports) threadend++;


---

3. Utility Classes

 stringutils - String Operations

class stringutils {
public:
     Convert to lowercase
    static string toLower(const string &str)
    
     Remove leading/trailing whitespace
    static string trim(const string &str)
    
     Check if string contains only digits
    static bool isnumber(const string &str)
};


---

 isvalidip - IP Address Validation

class isvalidip {
public:
     Check IPv4 format (e.g., "192.168.1.1")
    static bool isvalid(const string &ip)
    
     Check if localhost (127.0.0.1 or "localhost")
    static bool islh(const string &ip)
};


---

 progress - Progress Monitoring

class progress {
private:
    mutex statsmutex;   Protect statistics
public:
    void addopen()       Mark port as open
    void addclosed()     Mark port as closed
    void displayprogress()   Show progress bar
    void displaysummary()    Show final summary
};

Thread Safety: All methods use lock_guard to protect shared variables

---

 scannermanager - User Interface

class scannermanager {
private:
    struct scanconfig currentconfig;   Current settings
public:
    void run()   Main menu loop
};

Menu Options:
1. Configure Target IP
2. Configure Port Range
3. Configure Threads
4. Configure Timeout
5. Quick Scan
6. Common Services Scan
7. Run Scan
8. View Current Config
9. Help
0. Exit

---

 Platform Abstractions Layer

The code uses preprocessor directives to handle platform differences transparently.

Socket Type Definition

#ifdef _WIN32
    typedef SOCKET socket_t;   Windows: socket type is SOCKET
#else
    typedef int socket_t;      Linux: socket type is int
#endif


---

Cross-Platform Socket Close

inline void close_socket(socket_t s) {
#ifdef _WIN32
    closesocket(s);    Windows function
#else
    close(s);          POSIX function
#endif
}

Why? Different function names on different platforms

---

Cross-Platform Non-Blocking Mode
Windows Implementation:

u_long mode = 1;
ioctlsocket(s, FIONBIO, &mode);

- FIONBIO - Ioctl command for non-blocking mode
- mode - Set to 1 (enable) or 0 (disable)

Linux Implementation:

int flags = fcntl(s, F_GETFL, 0);   Get current flags
fcntl(s, F_SETFL, flags | O_NONBLOCK);   Add O_NONBLOCK flag

- F_GETFL - Get file descriptor flags
- F_SETFL - Set file descriptor flags
- O_NONBLOCK - Non-blocking flag constant

---

Cross-Platform IP Address Parsing
Windows:

inetPtonA(AF_INET, ip.c_str(), &out)
 Parameters:
 AF_INET - Address family (IPv4)
 ip.c_str() - IP string (e.g., "192.168.1.1")
 &out - Output binary address
 Returns: 1 (valid), 0 (invalid), -1 (error)


Linux:

inet_pton(AF_INET, ip.c_str(), &out)
 Same parameters and return values as Windows


---

Cross-Platform Error Checking
Checking for In-Progress Connection:

#ifdef _WIN32
    int e = WSAGetLastError();
    return e == WSAEWOULDBLOCK || e == WSAEINPROGRESS || e == WSAEALREADY;
#else
    return errno == EINPROGRESS;
#endif

Error Codes:
- Windows: WSAEWOULDBLOCK, WSAEINPROGRESS, WSAEALREADY
- Linux: EINPROGRESS

---

Privilege Detection

inline bool is_root() {
#ifdef _WIN32
    return false;   Windows privilege detection is complex
#else
    return geteuid() == 0;   Root UID is 0 on Unix
#endif
}


---

 Core Components Explained

Main Function Flow
1. Windows: Initialize Winsock (WSAStartup)
2. Setup signal handlers for Ctrl+C
3. Check if running with elevated privileges
4. Create ScannerManager
5. Run menu loop
6. Windows: Cleanup Winsock (WSACleanup)

---

Configuration Structure

struct scanconfig {
    string ip;         Target IP address
    int startport;     Starting port number
    int endport;       Ending port number
    int threads;       Number of worker threads
    int timeout;       Milliseconds per port
    
     Default constructor initializes to defaults
    scanconfig() 
        : ip("172.0.0.1"),
          startport(DEFAULT_START_PORT),
          endport(DEAFULT_END_PORT),
          threads(DEFAULT_THREADS),
          timeout(DEFULT_TIMEOUT_MS)
    {}
};


---

 Configuration Constants

const int DEFAULT_START_PORT = 1;       Minimum port
const int DEAFULT_END_PORT = 1024;      Well-known services range
const int DEFAULT_THREADS = 50;         Good default for most systems
const int DEFULT_TIMEOUT_MS = 500;      Milliseconds per port
const int MAX_THREADS = 200;            System sanity limit
const int MAX_PORT = 65535;             TCP/UDP maximum

Rationale:
- Ports 1-1024 are "well-known" service ports (FTP, SSH, HTTP, etc.)
- 50 threads provides good balance between speed and resources
- 500ms allows reasonable timeout on most networks
- Max 200 threads prevents resource exhaustion

---

 Usage Guide

Compilation
Linux:
bash
Compile with C++11 support and pthread
g++ -std=c++11 -pthread port_scanner. -o port_scanner

Run
./port_scanner


Windows (MinGW):
bash
Compile with Winsock2 library
g++ -std=c++11 port_scanner. -o port_scanner.exe -lws2_32

Run
port_scanner.exe


Requirements:
- g++/clang++ compiler supporting C++11
- Linux: pthread library (usually built-in)
- Windows: MinGW with Winsock2 support

---

Interactive Menu Example

===============================

     TCP PORT SCANNER  
     
===============================

=== MAIN MENU ===
1. Configure Target
2. Configure Port Range
3. Configure Threads
4. Configure Timeout
5. Quick Scan (1-1024)
6. Common Services Scan
7. Run Scan
8. View Current Config
9. Help
0. Exit

Enter choice: 1

--- Configure Target ---
Enter Target IP: [172.0.0.1] 192.168.1.100

Enter choice: 7

=== CURRENT CONFIGURATION ===

Target IP: 192.168.1.100
Port Range: 1 - 1024
Threads: 50
Timeout: 500ms

===============================

Starting Scan In 3 Seconds...
3... 2... 1... GO!

starting scan of 192.168.1.100...
port rage: 1 - 1024
threads: 50
time out: 500ms
press ctrl+c to stop scan

[=========>              ] 45.2% | Scanned: 462/1024 | Open: 3 | Closed: 459

[+] port    22 is open - SSH
[+] port    80 is open - HTTP
[+] port   443 is open - HTTPS

===SUMMARY===
total ports: 1024
open ports: 3
closed ports: 1021

===DETAILED RESULT===

PORT      SERVICE

-------------------------

22        SSH
80        HTTP
443       HTTPS


---

 Threading Model

Thread Creation & Synchronization

 Create worker threads
for (int i = 0; i < threadsn; i++) {
    workerthreads.emplace_back(&portscanner::workersf, this, i);
}

 Each thread executes workersf(i) independently

 Wait for all threads to complete
for (auto &thread : workerthreads) {
    if (thread.joinable()) 
        thread.join();   Blocks until thread finishes
}

Thread Execution:
1. Main thread creates N worker threads
2. Each worker scans assigned port range
3. Main thread blocks on .join() calls
4. When all workers finish, program continues

---

Data Synchronization
Shared Data That Needs Protection:

1. openports Vector

vector<int> openports;       Shared across threads
mutex resltsmutex;           Protects it

 Worker thread adds result
{
    lock_guard<mutex> lock(resltsmutex);
    openports.push_back(port);    Safe
}


2. Progress Statistics

void addopen() {
    lock_guard<mutex> lock(statsmutex);
    scannedports++;
    openports++;
}


3. Scanning Flag

atomic<bool> scanning(false);

 Check flag - no lock needed (atomic)
if (!scanning) break;


---

Atomic vs. Mutex
When to Use Atomic (for flags):

atomic<bool> scanning(false);
if (!scanning) break;   Very fast, no lock overhead
scanning = false;       Safe to update from main thread


When to Use Mutex (for complex data):

mutex resltsmutex;
{
    lock_guard<mutex> lock(resltsmutex);
    openports.push_back(port);   Must lock for vector operations
}


---

 Error Handling Strategy

Input Validation
IP Address Validation:

while (!isvalidip::isvalid(ip)) {
    cout << "invalid ip format\n";
    ip = getinput("Enter Target IP: ");
}
 Ensures only valid IPv4 addresses accepted


Port Range Validation:

if (minport < 1 || maxport > MAX_PORT || minport > maxport) {
    throw invalid_argument("inalid port range must be 1-65535");
}

if (minport > maxport) {
    cout << " Error The Start Port > The End Port Swap The Values ";
    swap(minport, maxport);
}


Thread Count Validation:

if (threads < 1 || threads > MAX_THREADS) {
    throw invalid_argument("thread cont must be 1-" + to_string(MAX_THREADS));
}


Timeout Validation:

if (timeout < 100 || timeout > 5000) {
    throw invalid_argument("timeout must be 100-5000 ms");
}


---

Exception Handling
Top-Level Try-Catch:

try {
    portscanner* scanner = new portscanner(currentconfig.ip, 
                                          currentconfig.startport,
                                          currentconfig.endport,
                                          currentconfig.threads,
                                          currentconfig.timeout);
    scanner->scan();
} catch (exception& e) {
    cout << "Error: " << e.what() << "\n";
} catch (...) {
    cout << "Unknown error occurred.\n";
}


---

Socket Error Handling
Connection Timeout Handling:

result = select((int)sockefd + 1, NULL, &writefds, NULL, &timeout);
if (result <= 0) {
     Timeout or error - port is closed
    return false;
}


Socket Creation Error:

socket_t sockefds = socket(AF_INET, SOCK_STREAM, 0);
#ifdef _WIN32
    if (sockefds == INVALID_SOCKET) return false;
#else
    if (sockefds < 0) return false;
#endif


---

 Performance Optimization

Thread Count Selection

| Scenario | Threads | Reason |
|----------|---------|---------|
| Local Network (LAN) | 100-200 | Low latency, can handle many concurrent |
| Home Internet | 50-100 | Moderate latency tolerance |
| Slow Network | 20-50 | Higher timeout needed, fewer threads |
| Many Ports (65535) | 150-200 | Maximize parallelism |
| Few Ports (1-100) | 10-20 | Thread creation overhead matters |
| Low-Resource System | 10-30 | Limit memory usage |

---

Timeout Selection

| Network Type | Timeout | Reason |
|-------------|---------|---------|
| Local LAN | 100-300ms | Immediate response expected |
| Fast Home Network | 300-500ms | Slightly more margin |
| Slow Internet | 1000-2000ms | RTT may be high |
| International | 2000-5000ms | Very high latency |

Trade-off: Higher timeout = more accurate but slower scan

---

Memory Usage Estimation

Memory ≈ (Threads × Stack Size) + (Results × 4 bytes)

Example Calculation:
- 100 threads × 8MB stack = 800MB
- 1000 open ports × 4 bytes = 4KB
- Total: ~800MB + overhead


Optimization Tips:
- More threads on multi-core systems
- Fewer threads on resource-constrained systems
- Use timeout to balance speed vs. accuracy

---

Progress Display Optimization

 Only update display every 10 ports (reduce overhead)
if (port % 10 == 0) {
    prog.displayprogress();
}

 Flush output for real-time display
cout.flush();


---

 Service Identification

The scanner identifies 14 common services by port number:

string ideserv(int port) const {
    if (port == 21) return "FTP";               File Transfer
    if (port == 22) return "SSH";               Secure Shell
    if (port == 23) return "Telnet";            Insecure remote
    if (port == 25) return "SMTP";              Email send
    if (port == 53) return "DNS";               Domain names
    if (port == 80) return "HTTP";              Web server
    if (port == 110) return "POP3";             Email retrieval
    if (port == 143) return "IMAP";             Email protocol
    if (port == 443) return "HTTPS";            Secure web
    if (port == 3306) return "MySQL";           Database
    if (port == 3389) return "RDP";             Remote desktop
    if (port == 5432) return "PostgreSQL";      Database
    if (port == 8080) return "HTTP Proxy";      Proxy server
    return "Unknown";
}


---

 Legal & Security Considerations

Legal Notice
⚠️ IMPORTANT - READ BEFORE USE

Unauthorized port scanning is illegal in many jurisdictions.

Only use this tool to scan:
- ✅ Systems you own
- ✅ Systems with written permission
- ✅ Systems you are authorized to test
- ✅ Local networks you manage
- ✅ Your own development/testing machines

Do NOT scan:
- ❌ Machines you dont own
- ❌ Machines without explicit permission
- ❌ Systems on networks you dont manage
- ❌ Systems on the public internet
- ❌ Systems for malicious purposes

---

Security Best Practices

1. Privilege Management

if (!is_root()) {
    cout << "Note you may need to run as root to scan ports < 1024\n";
}

- Linux: Run with sudo for ports 1-1024
- Windows: Run as Administrator for low ports
- High ports (1024+) dont require elevation

2. Timeout Configuration
- Higher timeout = slower scan but fewer false negatives
- Lower timeout = faster scan but may miss slow services

3. Thread Limits
- Too many threads can stress network
- Monitor system resources during scanning
- Use reasonable thread counts (20-100)

4. Network Awareness
- Scanning large port ranges is slow
- Scanning remote hosts is very slow (network latency)
- Local network scanning is much faster

---

Common Mistakes to Avoid
1. Scanning Without Permission - Most serious legal issue
2. Too Many Threads - Can crash your system or network
3. Very Low Timeout - Misses valid open ports
4. Not Verifying Target - Scanning wrong IP address
5. Not Checking Results - Running scan in background unnoticed

---

 Summary

This cross-platform TCP port scanner demonstrates:

✅ Multi-threaded design for efficient parallel scanning\
✅ Cross-platform abstraction for Windows/Linux compatibility\
✅ Thread synchronization with mutexes and atomic flags\
✅ Non-blocking I/O with select() timeout mechanism\
✅ User-friendly interface with interactive menu\
✅ Robust error handling with input validation\
✅ Object-oriented design with abstract interfaces\
✅ Responsive UI with progress tracking\
✅ Graceful shutdown on user interrupt\
✅ Service identification for common ports
