#include <netinet/in.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h>
#include <unistd.h> 
#include <arpa/inet.h> 

/* Server listens on this port. Maximum size for incoming data 1024 byte*/
#define PORT 5555 
#define BUFFER_SIZE 1024 

/* XOR-based encryption. The same function is used for both encryption/decryption */

void xor_crypt(char *data, int len, char key){
    for (int i = 0; i < len; i++) {
        data[i] ^= key;
    }
}

/* Simple integrity check via summing upp all bytes. Any change in the data will change the checksum. */
unsigned char checksum(const char *data, int len) {
    unsigned char sum = 0;
    for (int i = 0; i < len; i++) {
        sum += (unsigned char)data[i];
    }
    return sum;
}

int main(void) {
    int server_fd, client_fd; // File descriptors for the listening socker and accepted client. 
    
    /* Server addres (IP and port) */
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address); 

    char buffer[BUFFER_SIZE] = {0}; // Receive buffer initialized as zero.
    const char key = 0x5A; // Encryption key. Must match the key used by the client.

    /* 1. Create socket: IPv4, interface chosen port */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    /* 2. Bind the socket to the address and port. */
    address.sin_family = AF_INET; // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Lyssna på alla nätverkskort
    address.sin_port = htons(PORT); // port 5555

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    /* 3. Listen and ready to accept connections. */
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("[SERVER] Lyssnar på port %d...\n", PORT);

    /* 4. Block until a client connects. */
    client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);
    if (client_fd < 0) {
        perror("accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    /* 5. Read incoming data from the client. */
    int bytes = read(client_fd, buffer, BUFFER_SIZE - 1);
    if (bytes < 0) {
        perror("read failed");
        close(client_fd);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    /* 6. Verify the checksum (sista byten) */
    unsigned char received_sum = (unsigned char)buffer[bytes - 1];
    unsigned char calculated_sum = checksum(buffer, bytes - 1);

    if (received_sum != calculated_sum) {
        printf("[SERVER] Fel: checksumma stämmer inte!\n");
        close(client_fd);
        close(server_fd);
        return 1;
    }

    printf("[SERVER] Checksumma OK.\n");

    /* 7. Decrypt and terminate */
    xor_crypt(buffer, bytes - 1, key);
    buffer[bytes - 1] = '\0';

    printf("[SERVER] Dekrypterat meddelande: %s\n", buffer);

    /* 8. Close socket and exit. */
    close(client_fd);
    close(server_fd);
    return 0;
}