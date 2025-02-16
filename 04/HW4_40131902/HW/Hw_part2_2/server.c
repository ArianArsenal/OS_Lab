#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define MAX_PRODUCTS 100

typedef struct {
    char name[50];
    int quantity;
} Product;

typedef struct {
    int socket;
    char client_name[50];
    Product inventory[MAX_PRODUCTS];
    int product_count;
} ClientData;

ClientData clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t lock;

int find_client_index(const char *client_name) {
    for (int i = 0; i < client_count; i++) {
        if (strcmp(clients[i].client_name, client_name) == 0) {
            return i;
        }
    }
    return -1;
}

int find_product_index(ClientData *client, const char *product_name) {
    for (int i = 0; i < client->product_count; i++) {
        if (strcmp(client->inventory[i].name, product_name) == 0) {
            return i;
        }
    }
    return -1;
}

void handle_list(ClientData *client, int client_socket) {
    char response[BUFFER_SIZE] = "Inventory:\n";
    for (int i = 0; i < client->product_count; i++) {
        char line[100];
        sprintf(line, "%s: %d\n", client->inventory[i].name, client->inventory[i].quantity);
        strcat(response, line);
    }
    send(client_socket, response, strlen(response), 0);
}

void handle_create(ClientData *client, char *product_name, int quantity, int client_socket) {
    if (find_product_index(client, product_name) != -1) {
        send(client_socket, "Error: Product already exists.\n", 31, 0);
        return;
    }
    strcpy(client->inventory[client->product_count].name, product_name);
    client->inventory[client->product_count].quantity = quantity;
    client->product_count++;
    send(client_socket, "Product created successfully.\n", 30, 0);
}

void handle_add_reduce(ClientData *client, char *product_name, int amount, int client_socket, int is_add) {
    int product_index = find_product_index(client, product_name);
    if (product_index == -1) {
        send(client_socket, "Error: Product not found.\n", 27, 0);
        return;
    }
    int new_quantity = client->inventory[product_index].quantity + (is_add ? amount : -amount);
    if (new_quantity < 0) {
        send(client_socket, "Error: Insufficient stock.\n", 27, 0);
        return;
    }
    client->inventory[product_index].quantity = new_quantity;
    send(client_socket, is_add ? "Quantity added successfully.\n" : "Quantity reduced successfully.\n", 31, 0);
}

void handle_send(ClientData *sender, char *target_client_name, char *product_name, int amount, int client_socket) {
    int target_index = find_client_index(target_client_name);
    if (target_index == -1) {
        send(client_socket, "Error: Target client not found.\n", 31, 0);
        return;
    }

    int product_index = find_product_index(sender, product_name);
    if (product_index == -1 || sender->inventory[product_index].quantity < amount) {
        send(client_socket, "Error: Insufficient stock or product not found.\n", 47, 0);
        return;
    }

    sender->inventory[product_index].quantity -= amount;
    ClientData *receiver = &clients[target_index];
    int receiver_product_index = find_product_index(receiver, product_name);
    if (receiver_product_index == -1) {
        strcpy(receiver->inventory[receiver->product_count].name, product_name);
        receiver->inventory[receiver->product_count].quantity = amount;
        receiver->product_count++;
    } else {
        receiver->inventory[receiver_product_index].quantity += amount;
    }
    send(client_socket, "Transfer successful.\n", 21, 0);
}

void *client_handler(void *client_data) {
    ClientData *client = (ClientData *)client_data;
    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client->socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) break;

        char command[50], arg1[50];
        int arg2 = 0;
        sscanf(buffer, "%s %s %d", command, arg1, &arg2);

        pthread_mutex_lock(&lock);
        if (strcmp(command, "list") == 0) {
            handle_list(client, client->socket);
        } else if (strcmp(command, "create") == 0) {
            handle_create(client, arg1, arg2, client->socket);
        } else if (strcmp(command, "add") == 0 || strcmp(command, "reduce") == 0) {
            handle_add_reduce(client, arg1, arg2, client->socket, strcmp(command, "add") == 0);
        } else if (strcmp(command, "send") == 0) {
            char target_client_name[50], product_name[50];
            int amount;
            sscanf(buffer, "%*s %s %s %d", target_client_name, product_name, &amount);
            handle_send(client, target_client_name, product_name, amount, client->socket);
        } else if (strcmp(command, "quit") == 0) {
            break;
        } else {
            send(client->socket, "Error: Unknown command.\n", 24, 0);
        }
        pthread_mutex_unlock(&lock);
    }
    close(client->socket);
    return NULL;
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    listen(server_socket, MAX_CLIENTS);
    pthread_mutex_init(&lock, NULL);

    while (1) {
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket < 0) continue;

        char client_name[50];
        recv(client_socket, client_name, sizeof(client_name), 0);

        pthread_mutex_lock(&lock);
        if (find_client_index(client_name) != -1) {
            send(client_socket, "Error: Client name already in use.\n", 34, 0);
            close(client_socket);
            pthread_mutex_unlock(&lock);
            continue;
        }

        ClientData *new_client = &clients[client_count++];
        new_client->socket = client_socket;
        strcpy(new_client->client_name, client_name);
        new_client->product_count = 0;
        pthread_mutex_unlock(&lock);

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, client_handler, (void *)new_client);
        pthread_detach(thread_id);
    }

    pthread_mutex_destroy(&lock);
    close(server_socket);
    return 0;
}
