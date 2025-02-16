#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024
#define MAX_PRODUCTS 100

typedef struct {
    char name[50];
    int quantity;
} Product;

Product inventory[MAX_PRODUCTS];
int product_count = 0;

int find_product(const char *name) {
    for (int i = 0; i < product_count; i++) {
        if (strcmp(inventory[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void list_products(int client_socket) {
    char buffer[BUFFER_SIZE] = "Inventory List:\n";
    for (int i = 0; i < product_count; i++) {
        char product_info[100];
        snprintf(product_info, sizeof(product_info), "%s: %d\n", inventory[i].name, inventory[i].quantity);
        strcat(buffer, product_info);
    }
    send(client_socket, buffer, strlen(buffer), 0);
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) break;

        char command[50], name[50];
        int amount = 0;
        int args_read = sscanf(buffer, "%s %s %d", command, name, &amount);

        if (strcmp(command, "list") == 0) {
            list_products(client_socket);
        } 
        else if (strcmp(command, "create") == 0) {
            if (find_product(name) != -1) {
                send(client_socket, "Error: Product already exists.\n", 30, 0);
            } else {
                // Set quantity to zero if no initial count is provided
                if (args_read == 2) {
                    amount = 0;
                }
                if (amount < 0) {
                    send(client_socket, "Error: Quantity cannot be negative.\n", 36, 0);
                } else {
                    // Create the new product
                    inventory[product_count++] = (Product){.name = "", .quantity = amount};
                    strcpy(inventory[product_count - 1].name, name);
                    send(client_socket, "Product created successfully.\n", 30, 0);
                }
            }
        } 
        else if (strcmp(command, "add") == 0) {
            int index = find_product(name);
            if (index == -1) {
                send(client_socket, "Error: Product not found.\n", 27, 0);
            } else {
                inventory[index].quantity += amount;
                send(client_socket, "Quantity added successfully.\n", 29, 0);
            }
        } 
        else if (strcmp(command, "reduce") == 0) {
            int index = find_product(name);
            if (index == -1) {
                send(client_socket, "Error: Product not found.\n", 27, 0);
            } else if (inventory[index].quantity < amount) {
                send(client_socket, "Error: Insufficient stock.\n", 27, 0);
            } else {
                inventory[index].quantity -= amount;
                send(client_socket, "Quantity reduced successfully.\n", 31, 0);
            }
        } 
        else if (strcmp(command, "remove") == 0) {
            int index = find_product(name);
            if (index == -1 || inventory[index].quantity != 0) {
                send(client_socket, "Error: Product not removable.\n", 30, 0);
            } else {
                for (int i = index; i < product_count - 1; i++) {
                    inventory[i] = inventory[i + 1];
                }
                product_count--;
                send(client_socket, "Product removed successfully.\n", 30, 0);
            }
        } 
        else if (strcmp(command, "quit") == 0) {
            break;
        } 
        else {
            send(client_socket, "Error: Unknown command.\n", 24, 0);
        }
    }
    close(client_socket);
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [port]\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_socket, 1);

    printf("Server listening on port %d...\n", port);
    while (1) {
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket >= 0) {
            printf("Client connected.\n");
            handle_client(client_socket);
            printf("Client disconnected.\n");
        }
    }

    close(server_socket);
    return 0;
}
