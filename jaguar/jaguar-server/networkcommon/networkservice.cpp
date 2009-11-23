#include <string.h>

#include "util.h"
#include "networkservice.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "iostream"
#include "sstream"
#include <map>
#include <fcntl.h>
#include "dbjaguar.h"

using namespace dbjaguar;

// Just declared to be used later
void *startSocketListener(void* arg);
void *processRequest(void* arg);

Logger* log;
bool running;
bool accepting;
// id listening socket
int sock;


Connection* m_con;

// Scripts addition
extern void AddWorkflow();
extern void loadProcessDefinitions(Connection* con);

void initializeDatabase() throw (DBException) {
    if (log->isInfo()) log->info("Initializing DB. Connecting using: mysql;localhost;3304;jaguarmd");
    ConnectionPool* pool = new ConnectionPool();
    m_con = pool->getConnection("mysql;localhost;3304;jaguarmd", "root", "cross2000");
    delete pool;
}

void initializeProcessDefinitions() {
    loadProcessDefinitions(m_con);
}

NetworkService::NetworkService() {
    log = getLogger(NULL);
}

void registerControllers() {
    if (log->isDebug()) log->debug("Registering controllers");

    AddWorkflow();
}

void NetworkService::start() throw (NetworkException) {
    if (running) {
        throw NetworkException(new string("The network service is already active. Try stopping it first"));
    }
    if (log->isInfo()) log->info("Starting network service");

    registerControllers();
    initializeDatabase();
    initializeProcessDefinitions();

    Thread* thread = new Thread(&startSocketListener);
    thread->start(NULL);

}

void NetworkService::stop() throw (NetworkException) {
    log->info("Shutting down the network service");
    if (!running) {
        throw NetworkException(new string("The network service is not running. Try starting it first"));
    }
    running = false;
    while (accepting) {
        sleep(1);
    }
    int res = close(sock);
    m_con->close();
    if (res != 0) {
        log->error("The close method returned: " + toString(res));
    }
}

void *startSocketListener(void* arg) {
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        log->error("Error creating the socked");
    }

    sockaddr_in* addr = new sockaddr_in();
    int port = 1043;
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port); // the port should be converted to network byte order
    addr->sin_addr.s_addr = INADDR_ANY; // Server address, any to take the current ip address of the host

    if (bind(sock, (sockaddr *) addr, sizeof(*addr) ) < 0) {
        log->error("Error binding");
    }
    listen(sock, 5);
    log->info("Accepting new connections");
    running = true;
    // Sets the nonblocking option for this socket
    int currentFlag = fcntl(sock, F_GETFD);
    currentFlag = currentFlag | O_NONBLOCK;
    fcntl(sock, F_SETFL, currentFlag);

    accepting = true;
    while (running) {
        sockaddr_in cliaddr;
        socklen_t clilen = sizeof (cliaddr);
        int newsocket = accept(sock, (sockaddr *)&cliaddr, &clilen);
        fd_set read;
        FD_ZERO (&read);
        FD_SET (sock, &read);
        timeval val;
        val.tv_sec = 1;
        val.tv_usec = 0;

        newsocket = select(sock+1, &read, NULL, NULL, &val);

        if (newsocket > 0) {
            newsocket = accept(sock, (sockaddr *)&cliaddr, &clilen);
            log->debug("Accepted");
            processRequest(&newsocket);
//            Thread* thread = new Thread(&processRequest);
//            thread->start((void*)&newsocket);
        }
    }
    accepting = false;

    return NULL;
};


void *processRequest(void *arg) {
    int clientSocket = *((int*)arg);
    if (log->isDebug()) log->debug("Receiving request");

    int readed;
    stringstream sreaded;

    char buffer[256];
    memset(buffer, 0, 256);

    bool reachEnd = false;
    // Reads the socket data until the socket sends the end signal 'FFFF'
    while ( !reachEnd) {
        fd_set read;
        FD_ZERO (&read);
        FD_SET (sock, &read);
        timeval val;
        val.tv_sec = 10;
        val.tv_usec = 0;

        readed = recv(clientSocket, buffer, 255, 0);
        if (readed < 0) {
            if (log->isDebug()) log->debug("Readed: " + toString(readed));
            int result = select(clientSocket+1, &read, NULL, NULL, &val);

            if (log->isDebug()) log->debug("result: " + toString(result));
            if (result < 0) {
                log->info("Timeout");
                close(clientSocket);
                return NULL;
            } else {
                readed = recv(clientSocket, buffer, 255, 0);
            }
        }
        sreaded << buffer;
        string s = sreaded.str();
        string::reverse_iterator it;
        it = s.rbegin();
        for (int x = 0; x < 4; x++) {
            char c = *it;
            if (c != 'F') {
                break;
            }
            it++;
            if (x == 3)
                reachEnd = true;
        }
        memset(buffer, 0, 256);
    }

    if (log->isDebug()) log->debug("Buffer received, size: " + toString(readed));

    Request* request = new Request(sreaded.str().c_str());

    RequestProcessor* processor = new RequestProcessor();
    Response* response = processor->processRequest(request);

    string* sresp = response->getData();

    write(clientSocket, sresp->c_str(), sresp->length());
    close(clientSocket);
    return NULL;
};


