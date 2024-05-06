#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define PORT 5000

int main() {
    int client;
    struct sockaddr_in server;
    char buffer[2048];

  // Membuat socket client
    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client < 0) {
        printf("Error creating socket\n");
        return -1;
    }
  
  // Mengisi informasi server
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
   
  // Membuat koneksi ke server
    if (connect(client, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Error connecting to server\n");
        return -1;
    }
  
    // Loop untuk terus menerima dan mengirim pesan antara klien dan server
    while (1) {
        printf("You: ");
        memset(buffer, 0, sizeof(buffer));
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        send(client, buffer, strlen(buffer), 0);
      
        // Memeriksa apakah pengguna ingin keluar dari aplikasi
        if (strcmp(buffer, "exit") == 0) {
           exit(0);
        }
      
        // Mengosongkan buffer sebelum menerima respons dari server
        memset(buffer, 0, sizeof(buffer)); 
        // Menerima respons dari server
        int getbytes = recv(client, buffer, sizeof(buffer), 0); 
        if (getbytes <= 0) {
            printf("Connection closed by server\n");
            break;
        }
        printf("Server:\n%s\n", buffer);
    }
    printf("Exiting the client\n");
    close(client);
    return 0;
}
