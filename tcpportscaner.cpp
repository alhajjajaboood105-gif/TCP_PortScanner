#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <algorithm>
#include <iomanip>
#include <csignal>
#include <atomic>
#include <chrono>
#ifdef _WIN32
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
typedef SOCKET socket_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
typedef int socket_t;
#endif



using namespace std;

const int DEFAULT_START_PORT = 1;
const int DEAFULT_END_PORT = 1024;
const int DEFAULT_THREADS = 50;
const int DEFULT_TIMEOUT_MS = 500;
const int MAX_THREADS = 200;
const int MAX_PORT = 65535;
inline void close_socket(socket_t s) {
#ifdef _WIN32
    closesocket(s);
#else
    close(s);
#endif
}
inline bool set_nonblocking(socket_t s) {
#ifdef _WIN32
    u_long mode = 1;
    return ioctlsocket(s, FIONBIO, &mode) == 0;
#else 
    int flags = fcntl(s, F_GETFL, 0);
    if (flags < 0) return false;
    return fcntl(s, F_SETFL, flags | O_NONBLOCK) == 0;
#endif
}
inline void sleep_ms(int ms) {
    this_thread::sleep_for(chrono::milliseconds(ms));
}
inline bool inet_pton_compat(const string& ip, struct in_addr& out) {
#ifdef _WIN32
    return InetPtonA(AF_INET, ip.c_str(), &out) == 1;
#else
    return inet_pton(AF_INET, ip.c_str(), &out) == 1;
#endif
}
inline bool is_root() {
#ifdef _WIN32
    return false;
#else
    return geteuid() == 0;
#endif
}
inline bool isprograes_error() {
#ifdef _WIN32
    int e = WSAGetLastError(); 
    return e == WSAEWOULDBLOCK || e == WSAEINPROGRESS || e == WSAEALREADY;
#else
    return errno == EINPROGRESS;
#endif
}
class stringutils {
public:
    static string toLower(const string& str) {
        string result = str;
        transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    static string trim(const string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == string::npos)return "";
        size_t last = str.find_last_not_of("\t\n\r");
        return str.substr(first, (last - first + 1));
    }
    static bool isnumber(const string& str) {
        if (str.empty()) return false;
        for (char c : str)if (!isdigit(c))return false;
        return true;
    }
};
class isvalidip {
public:
    static bool isvalid(const string& ip) {
        struct in_addr addr;
        return inet_pton_compat(ip, addr);
    }
    static bool islh(const string& ip) {
        return ip == "172.0.0.1" || ip == "localhost";
    }
};
class scannerabs {
public:
    virtual void scan() = 0;
    virtual void stop() = 0;
    virtual vector <int>getResults() const = 0;
    virtual ~scannerabs() {}
};
class stratgyabs {
public:
    virtual bool checkport(const string& ip, int port, int timeout_ms) = 0;
    virtual ~stratgyabs() {}
};
class tcpstratgy : public stratgyabs {
private:
    bool connectwithtimeout(socket_t sockefd, sockaddr_in addr, int timeoutms) {
        if (!set_nonblocking(sockefd)) return false;
        int result = connect(sockefd, (struct sockaddr*)&addr, sizeof(addr));

#ifdef _WIN32
        if (result == 0) return true;
        if (!isprograes_error())return false;
#else 
        if (result == 0) return true;
        if (errno != EINPROGRESS) return false;
#endif 
        fd_set writefds;
        FD_ZERO(&writefds);
        FD_SET((unsigned int)sockefd, &writefds);
        struct timeval timeout;
        timeout.tv_sec = timeoutms / 1000;
        timeout.tv_usec = (timeoutms % 1000) * 1000;
        result = select((int)sockefd + 1, NULL, &writefds, NULL, &timeout);
        if (result <= 0) return false;
        int error = 0;
        socklen_t len = sizeof(error);
        if (getsockopt(sockefd, SOL_SOCKET, SO_ERROR, (char*)&error, &len) < 0)return false;
#ifdef _WIN32
        return error == 0 || error == WSAEWOULDBLOCK;
#else
        return error == 0;
#endif
    }
public:
    bool checkport(const string& ip, int port, int timeoutms) override {
        socket_t sockefds = socket(AF_INET, SOCK_STREAM, 0);
#ifdef _win32
        if (sockfd == INVALID_SOCKET) return false;
#else 
        if (sockefds < 0) return false;
#endif
        struct sockaddr_in target_addr;
        memset(&target_addr, 0, sizeof(target_addr));
        target_addr.sin_family = AF_INET;
        target_addr.sin_port = htons(port);
        if (!inet_pton_compat(ip, target_addr.sin_addr)) {
            close_socket(sockefds);
            return false;
        }
        bool isopen = connectwithtimeout(sockefds, target_addr, timeoutms);
        close_socket(sockefds);
        return isopen;
    }
};
class progress {
private:
    int totalports;
    int scannedports;
    int openports;
    int closedports;
    mutex statsmutex;
public:
    progress() :totalports(0), scannedports(0), openports(0), closedports(0) {}
    void setTotal(int total) {
        lock_guard<mutex>lock(statsmutex);
        totalports = total;
    }
    void addopen() {
        lock_guard<mutex>lock(statsmutex);
        scannedports++;
        openports++;
    }
    void addclosed() {
        lock_guard<mutex>lock(statsmutex);
        scannedports++;
        closedports++;
    }
    void displayprogress() {
        lock_guard<mutex>lock(statsmutex);
        float pecentage = totalports > 0 ? (scannedports * 100.0f / totalports) : 0;
        cout << "\r[";
        int barwidth = 50;
        int pos = barwidth * pecentage / 100.0;
        for (int i = 0; i < barwidth; ++i) {
            if (i < pos) cout << "=";
            else if (i == pos) cout << ">";
            else cout << " ";
        }
        cout << "] " << fixed << setprecision(1) << pecentage << "%";
        cout << " | Scanned : " << scannedports << "/" << totalports;
        cout << "Open : " << openports << " | Closed : " << closedports << flush;
    }
    void displaysummary() {
        lock_guard<mutex>lock(statsmutex);
        cout << "\n\n===SUMMARY===\n";
        cout << "total ports : " << scannedports << "\n";
        cout << "open ports : " << openports << "\n";
        cout << "closed ports : " << closedports << "\n";
        if (openports > 0) cout << "\nopen ports found : " << openports << "\n";
        else cout << "\nno open ports founded\n";
    }
    void reset() { lock_guard<mutex> lock(statsmutex); scannedports = 0; closedports = 0; }
    int getopenports()const { return openports; }
};
class portscanner : public scannerabs {
private:
    string targetip;
    int startport;
    int endport;
    int threadsn;
    int timeoutms;
    atomic<bool>scanning;
    vector<thread>workerthreads;
    vector<int>openports;
    mutex resltsmutex;
    progress prog;
    stratgyabs* scanstratgy;
    string ideserv(int port) const {
        if (port == 21)return"FTP";
        if (port == 22)return "SSH";
        if (port == 23)return "Telnet";
        if (port == 25)return "SMTP";
        if (port == 53)return "DNS";
        if (port == 80)return"HTTP";
        if (port == 110)return "POP3";
        if (port == 143)return "IMAP";
        if (port == 443)return "HTTPS";
        if (port == 3306)return "MySQL";
        if (port == 3389)return "RDP";
        if (port == 5432)return "PostgreSQL";
        if (port == 8080)return "HTTP Proxy";
        return "Unknown";
    }
    void scanportrange(int start, int end) {
        for (int port = start; port <= end; ++port) {
            if (!scanning)break;
            if (scanstratgy->checkport(targetip, port, timeoutms)) {
                lock_guard<mutex>lock(resltsmutex);
                openports.push_back(port);
                prog.addopen();
                cout << "\n[+] port " << setw(5) << port << "is open -" << ideserv(port);
            }
            else {
                prog.addclosed();
            }
            if (port % 10 == 0)prog.displayprogress();
        }
    }
    void workersf(int threadindex) {
        int totalports = endport - startport + 1;
        if (totalports <= 0) return;
        int portsPerthread = totalports / threadsn;
        int extraports = totalports % threadsn;
        int offset = threadindex * portsPerthread + min(threadindex, extraports);
        int threadstart = startport + offset;
        int threadend = threadstart + portsPerthread - 1;
        if (threadindex < extraports)threadend++;
        if (threadstart > threadend)return;
        scanportrange(threadstart, threadend);
    }
public:
    portscanner(const string& ip, int minport, int maxport, int threads, int timeout) :targetip(ip), startport(minport), endport(maxport), threadsn(threads), timeoutms(timeout), scanning(false) {
        if (!isvalidip::isvalid(ip)) throw invalid_argument("invalid ip format");
        if (minport <1 || maxport>MAX_PORT || minport > maxport) throw invalid_argument("inalid port range must be 1-65535");
        if (threads<1 || threads>MAX_THREADS)throw invalid_argument("thread cont must be 1-" + to_string(MAX_THREADS));
        if (timeout < 100 || timeout >5000) throw invalid_argument("timeout must be 100-5000 ms");
        scanstratgy = new tcpstratgy();
        prog.setTotal(maxport - minport + 1);
    }
    void scan()override {
        scanning = true;
        openports.clear();
        prog.reset();
        prog.setTotal(endport - startport + 1);
        cout << "\nstarting scan of " << targetip << "\n";
        cout << "port rage : " << startport << " - " << endport << "\n";
        cout << "threads : " << threadsn << "\n";
        cout << "time out : " << timeoutms << "ms\n";
        cout << "press ctrl+c to stop scan\n\n";
        for (int i = 0; i < threadsn; i++) workerthreads.emplace_back(&portscanner::workersf, this, i);
        for (auto& thread : workerthreads) if (thread.joinable()) thread.join();
        scanning = false;
        workerthreads.clear();
        prog.displayprogress();
        prog.displaysummary();
        displayopenports();
    }
    void stop()override {
        scanning = false;
        for (auto& thread : workerthreads)if (thread.joinable()) thread.join();
        workerthreads.clear();
    }
    vector <int> getResults()const override { return openports; }
    void displayopenports()const {
        if (openports.empty()) return;
        cout << "\n===DETAILED RESULT===\n";
        cout << left << setw(10) << "PORT" << "SERVICE\n";
        cout << string(25, '-') << "\n";
        vector<int> sortedport = openports;
        sort(sortedport.begin(), sortedport.end());
        for (int port : sortedport) cout << left << setw(10) << port << ideserv(port) << "\n";
    }
    ~portscanner() { stop(); delete scanstratgy; };
};
class scannermanager {
private:
    struct scanconfig { string ip; int startport; int endport; int threads; int timeout; scanconfig() : ip("172.0.0.1"), startport(DEFAULT_START_PORT), endport(DEAFULT_END_PORT), threads(DEFAULT_THREADS), timeout(DEFULT_TIMEOUT_MS) {} };
    scanconfig currentconfig;
    void displayheader() {
        cout << "\n==============================\n";
        cout << "     TCP PORT SCANNER    ";
        cout << "\n==============================\n";
    }
    void displaycurrenconfig() {
        cout << "\n=== CURRENT CONFIGURATION ===\n";
        cout << "Target IP : " << currentconfig.ip << "\n";
        cout << "Port Range : " << currentconfig.startport << " - " << currentconfig.endport << "\n";
        cout << "Threads : " << currentconfig.threads << "\n";
        cout << "Timeout : " << currentconfig.timeout << "ms" << "\n";
        cout << "\n==============================\n";
    }
    string getinput(const string& prompt, const string& defultvalue = "") {
        cout << prompt;
        if (!defultvalue.empty()) cout << "[" << defultvalue << "] ";
        string input; getline(cin, input); input = stringutils::trim(input);
        if (input.empty() && !defultvalue.empty())return defultvalue;
        return input;
    }
    int getnumberinput(const string& prompt, int defaultvalue, int min, int max) {
        while (true) {
            string input = getinput(prompt, to_string(defaultvalue));
            if (!stringutils::isnumber(input)) { cout << "please enter anumber\n"; continue; }
            int value = stoi(input);
            if (value<min || value>max) {
                cout << "please enter a number between " << min << " and " << max << "\n";
                continue;
            }
            return value;
        }
    }
    void configtarget() {
        cout << "\n --- Configre Target ---\n";
        string ip = getinput("Enter Target IP : ", currentconfig.ip);
        while (!isvalidip::isvalid(ip)) {
            cout << "invalid ip format\n";
            ip = getinput("Enter Target IP : ", currentconfig.ip);
        }
        currentconfig.ip = ip;
        if (isvalidip::islh(ip)) cout << "Scanning localhost. may you need root privileges\n";
    }
    void configportrange() {
        cout << "\n---Configure Port Range---\n";
        currentconfig.startport = getnumberinput("Start port (1-65535) : ", currentconfig.startport, 1, MAX_PORT);
        currentconfig.endport = getnumberinput("End Port (1-65535) : ", currentconfig.endport, 1, MAX_PORT);
        if (currentconfig.startport > currentconfig.endport) {
            cout << " Error The Start Port > The End Port Swap The Values ", swap(currentconfig.startport, currentconfig.endport);
        }
        int totalports = currentconfig.endport - currentconfig.startport + 1;
        cout << "Will Scan " << totalports << " ports\n";
    }
    void configthreads() {
        cout << "\n---Config Treads---\n";
        currentconfig.threads = getnumberinput("Nuber Of threads (1-" + to_string(MAX_THREADS) + ") : ", currentconfig.threads, 1, MAX_THREADS);
    }
    void configtimeout() {
        cout << "/n--- Config Timeout ---\n";
        currentconfig.timeout = getnumberinput("Timeout (100-5000) ms : ", currentconfig.timeout, 100, 5000);
    }
    void quickscan() {
        cout << "\n---Quick Scan---\nScanning common ports (1-1024)...";
        currentconfig.startport = 1;
        currentconfig.endport = 1024;
        currentconfig.threads = 50;
        currentconfig.timeout = 500;
        runscan();
    }
    void commonservscan() {
        cout << "\n---Cpmmon Services Scan---\n";
        vector<int>commonport = { 21,22,23,25,53,80,110,143,443,445,993,995,1723,3306,3389,5900,8080 };
        cout << "\nScanning " << commonport.size() << "Common Service Port";
        cout << "\nPort\tStatus\tService\n";
        cout << "\n----------------------------\n";
        for (int port : commonport) {
            tcpstratgy stratgy;
            bool isopen = stratgy.checkport(currentconfig.ip, port, currentconfig.timeout);
            string service = "Unknown";
            if (port == 21) service = "FTP"; else if (port == 22) service = "SSH"; else if (port == 23) service = "Telnet"; else if (port == 25) service = "SMTP"; else if (port == 53) service = "DNS"; else if (port == 80) service = "HTTP"; else if (port == 110) service = "POP3"; else if (port == 143) service = "IMAP"; else if (port == 443) service = "HTTPS"; else if (port == 445) service = "SMB"; else if (port == 993) service = "IMAPS"; else if (port == 995) service = "POP3S"; else if (port == 1723) service = "PPTP"; else if (port == 3306) service = "MySQL"; else if (port == 3389) service = "RDP"; else if (port == 5900) service = "VNC"; else if (port == 8080) service = "HTTP Proxy";
            cout << setw(5) << port << "\t" << (isopen ? "OPEN" : "CLOSE") << "\t" << service << "\n";
            cout.flush();
            sleep_ms(10);
        }
        cout << "\nCommon Services Scan Complete\n";
    }
    void runscan() {
        displaycurrenconfig();
        cout << "\nStarting Scan In 3 Seconds...\n";
        for (int i = 3; i > 0; i--) {
            cout << i << "...";
            cout.flush();
            sleep_ms(1000);
        }
        cout << "GO\n";
        portscanner* scanner = nullptr;
        try {
            scanner = new portscanner(currentconfig.ip, currentconfig.startport, currentconfig.endport, currentconfig.threads, currentconfig.timeout);
            scanner->scan();
        }
        catch (exception& e) {
            cout << "Error : " << e.what() << "\n";
        }
        delete scanner;
    }
    void displayhelp() {
        cout << "\n=== HELP ===\n";
        cout << "1. Configure Target - Set the IP address to scan\n";
        cout << "2. Configure Port Range - Set which ports to scan (1-65535)\n";
        cout << "3. Configure Threads - Set number of concurrent scanning threads\n";
        cout << "4. Configure Timeout - Set timeout per port in milliseconds\n";
        cout << "5. Quick Scan - Scan common ports (1-1024) with default settings\n";
        cout << "6. Common Services Scan - Scan only well-known service ports\n";
        cout << "7. Run Scan - Start scanning with current configuration\n";
        cout << "8. View Current Config - Display current settings\n";
        cout << "9. Help - Display this help message\n";
        cout << "0. Exit - Exit the program\n";
        cout << "\nTips:\n- Be respectful and only scan systems you own or have permission to scan\n";
    }
public:
    scannermanager() {
        displayheader();
        cout << "intlizing scanner...\n";
        cout << "Type 'help' or '9' for help at any time \n";
    }
    void run() {
        while (true) {
            cout << "\n===MAIN MENU===\n";
            cout << "1.Configure Target\n2.Configure Port Range\n3.Configure Threads\n4.Configure Timeout\n5.Quick Scan\n6.Common Services Scan\n7.Run Scan\n8.View Curent Config\n9.Help\n0.Exit\n";
            string choice = getinput("\nEnter Choice : ");
            choice = stringutils::toLower(choice);
            if (choice == "0" || choice == "exit" || choice == "quit") { cout << "\nGoodbye!\n"; break; }
            if (choice == "help" || choice == "9") { displayhelp(); continue; }
            int option = stoi(choice);
            try { switch (option) { case 1: configtarget(); break; case 2: configportrange(); break; case 3: configthreads(); break; case 4: configtimeout(); break; case 5: quickscan(); break; case 6: commonservscan(); break; case 7: runscan(); break; case 8: displaycurrenconfig(); break; default: cout << "Invalid option. Please try again.\n"; } }
                                          catch (const exception& e) { cout << "Error: " << e.what() << "\n"; }
                                          catch (...) { cout << "Unknown error occurred.\n"; }
        }
    }
};
class signalhndler {
private:
    static bool interrupted;
public:
    static void setup() {
        signal(SIGINT, handleSignal);
        signal(SIGTERM, handleSignal);
    }
    static void handleSignal(int) {
        interrupted = true;
        cout << "\n\nScan Interrupted By User\n";
    }
    static bool isinterrupted() {
        return interrupted;
    }
    static void reset() {
        interrupted = false;
    }
};
bool signalhndler::interrupted = false;
int main() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSASrartup failed\n";
        return 1;
    }
#endif
    try {
        signalhndler::setup();
        if (!is_root()) {
            cout << "Note you may need to run as root to scan ports < 1024\n Press Enter To Continue or CTRL+c to Exit...";
            cin.ignore();
        }
        scannermanager manager;
        manager.run();
    }
    catch (const exception& e) {
        cout << "Fatal Error : " << e.what() << "\n";
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }
#ifdef _WIN32
    WSACleanup();
#endif
        return 0;

}