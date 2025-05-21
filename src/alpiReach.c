#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>    // For getaddrinfo
#include <unistd.h>   // For close, fcntl
#include <fcntl.h>    // For fcntl, O_NONBLOCK
#include <sys/time.h> // For struct timeval (used by select)
#include <errno.h>    // For errno

#define CONNECTION_TIMEOUT_SECONDS 3

int is_valid_integer(const char *str)
{
    if (str == NULL || *str == '\0')
        return 0;
    while (*str)
    {
        if (!isdigit((unsigned char)*str))
            return 0;
        str++;
    }
    return 1;
}

int main(int argc, char *argv[])
{
    int _timeout;
    int hostArgBase = 0;

    if (argc > 1 && is_valid_integer(argv[1]))
    {
        _timeout = atoi(argv[1]);
        hostArgBase = 1;
    }
    else
    {
        _timeout = CONNECTION_TIMEOUT_SECONDS;
    }

    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv; // Return value for getaddrinfo
    int flags;
    fd_set fdset;
    struct timeval tv;
    int so_error; // Socket error
    socklen_t len;

    // --- Argument Validation ---
    if (!(argc == 3 || argc == 4))
    {
        fprintf(stderr, "Usage: %s <hostname> <port>\n", argv[0]);
        fprintf(stderr, "Example: %s example.com 80\n", argv[0]);
        return 2; // Specific exit code for usage error
    }

    const char *host = argv[1 + hostArgBase];
    const char *port_str = argv[2 + hostArgBase];

    // Validate port is a number (basic check)
    for (int i = 0; port_str[i] != '\0'; i++)
    {
        if (port_str[i] < '0' || port_str[i] > '9')
        {
            fprintf(stderr, "Error: Port must be a positive integer.\n");
            return 2;
        }
    }
    int port_num = atoi(port_str);
    if (port_num <= 0 || port_num > 65535)
    {
        fprintf(stderr, "Error: Port number must be between 1 and 65535.\n");
        return 2;
    }

    // --- Prepare hints for getaddrinfo ---
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;     // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP socket

    // --- Resolve Hostname ---
    // printf("Resolving %s...\n", host);
    if ((rv = getaddrinfo(host, port_str, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "Error: getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // --- Loop through results and attempt connection ---
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        // Create socket
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            // perror("socket"); // Print system error message for socket
            continue; // Try next address
        }

        // Set socket to non-blocking
        flags = fcntl(sockfd, F_GETFL, 0);
        if (flags == -1)
        {
            // perror("fcntl(F_GETFL)");
            close(sockfd);
            continue;
        }
        if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1)
        {
            // perror("fcntl(F_SETFL, O_NONBLOCK)");
            close(sockfd);
            continue;
        }

        // Initiate connection (non-blocking)
        // printf("Attempting to connect to an address for %s on port %s...\n", host, port_str);
        rv = connect(sockfd, p->ai_addr, p->ai_addrlen);

        if (rv == 0)
        {
            // Connection established immediately (unlikely for non-loopback, but possible)
            printf("SUCCESS: Port %s on %s is reachable.\n", port_str, host);
            close(sockfd);
            freeaddrinfo(servinfo); // Free the linked list
            return 0;
        }
        else if (rv == -1 && errno == EINPROGRESS)
        {
            // Connection attempt is in progress
            FD_ZERO(&fdset);
            FD_SET(sockfd, &fdset);

            tv.tv_sec = _timeout;
            tv.tv_usec = 0;

            // Wait for connection to complete or timeout
            rv = select(sockfd + 1, NULL, &fdset, NULL, &tv);

            if (rv == 1)
            { // Socket became writable (connected or error)
                len = sizeof so_error;
                getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &so_error, &len);

                if (so_error == 0)
                {
                    printf("SUCCESS: Port %s on %s is reachable.\n", port_str, host);
                    close(sockfd);
                    freeaddrinfo(servinfo);
                    return 0;
                }
                else
                {
                    // Connection failed after select
                    // fprintf(stderr, "Error connecting after select: %s\n", strerror(so_error));
                    close(sockfd);
                    continue; // Try next address
                }
            }
            else if (rv == 0)
            {
                // Timeout occurred
                // fprintf(stderr, "Timeout connecting to %s on port %s.\n", host, port_str);
                close(sockfd);
                continue; // Try next address
            }
            else
            { // rv == -1
                // Error in select
                // perror("select");
                close(sockfd);
                continue; // Try next address
            }
        }
        else
        {
            // Immediate connection error (e.g., network unreachable, before EINPROGRESS)
            // perror("connect");
            close(sockfd);
            continue; // Try next address
        }
    } // End for loop

    // If we got here, we couldn't connect to any address
    fprintf(stderr, "FAILURE: Could not connect to %s on port %s.\n", host, port_str);
    freeaddrinfo(servinfo); // Free the linked list
    return 1;
}
