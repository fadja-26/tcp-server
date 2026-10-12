# Encrypted TCP CLient and Server in C 

A small client-server project demonstrating low-level network programming in C on Linux.

## Features 

- TCP sockets in Linux user space
- XOR-based symmetric encryption
- SImple checksum for integrity verification 
- Error handling on all system calls

## How to build 
gcc -Wall -o server server.c

gcc - Wall - client client.c

# How to run

Terminal 1:
./server

Terminal 2:
./client

## Expected output 

Server:
[SERVER] Listening on port 5555...
[SERVER] Client connected.
[SERVER] Checksum OK.
[SERVER] Decrypted message: Hello world!

Client:
[CLIENT] Connected to server.
[CLIENT] ENcrypted message sent (13 bytes).

## Notes

The XOR key must match on both sides.
