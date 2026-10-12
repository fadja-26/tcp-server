#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5555  // Must match the server's port
#define BUFFER_SIZE 1024 // Must match the server's buffer size

void xor_crypt(char *data, int len, char key) {
    for (int i = 0; i < len; i++) {
        data[i] ^= key;
    }
}

unsigned char checksum(const char *data, int len) {
    unsigned char sum = 0;
    for (int i = 0; i < len; i++) {
        sum += (unsigned char)data[i];
    }
    return sum;
}

int main(void) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    const char key = 0x5A;
    const char *message = "Hello world!"; // Message to send

    // Create a TCP socket for IPv4.
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert the IP string to binary form. 
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("inet_pton failed");
        exit(EXIT_FAILURE);
    }

    // Connect to the server (TCP handshake)
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    printf("[KLIENT] Ansluten till servern.\n");

    // Copy the message into the buffer
    int len = strlen(message);
    memcpy(buffer, message, len);

    // Encrypt the message
    xor_crypt(buffer, len, key);

    buffer[len] = (char)checksum(buffer, len);

    // Send the encrypted message plus the cheksum
    send(sock, buffer, len + 1, 0);
    printf("[KLIENT] Krypterat meddelande skickat (%d bytes).\n", len + 1);

    // Close the socked and exit program. 
    close(sock);
    return 0;
}