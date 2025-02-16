#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server-ip> <server-port> <client-name>\n", argv[0]);
        return 1;
    }

    // Server connection details
    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);
    char *client_name = argv[3];

    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip, &server_address.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        close(sock);
        return 1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        close(sock);
        return 1;
    }

    // Send client name to server for identification
    send(sock, client_name, strlen(client_name), 0);

    char command[BUFFER_SIZE];
    char server_response[BUFFER_SIZE];

    while (1) {
        // Prompt user for a command
        printf("\nEnter command (list, create, add, reduce, remove, send, quit): ");
        fgets(command, BUFFER_SIZE, stdin);
        command[strcspn(command, "\n")] = '\0';  // Remove newline character

        // Send command to server
        send(sock, command, strlen(command), 0);

        // If quit command, close connection and exit
        if (strcmp(command, "quit") == 0) {
            printf("Disconnecting from server...\n");
            break;
        }

        // Receive response from server
        int bytes_received = recv(sock, server_response, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            printf("Server disconnected.\n");
            break;
        }

        server_response[bytes_received] = '\0';  // Null-terminate the response
        printf("Server response:\n%s\n", server_response);
    }

    close(sock);
    return 0;
}
