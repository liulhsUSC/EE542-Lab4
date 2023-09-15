#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <arpa/inet.h>

#define PORT 8080
#define DATA_SIZE 1024  // Based on your desired MTU <=============
#define HEADER_SIZE sizeof(int)

struct Packet {
    int seq_num;
    char data[DATA_SIZE];
};

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[sizeof(Packet)];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    std::memset(&servaddr, 0, sizeof(servaddr));
    std::memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    std::ofstream file("received_data.bin", std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing.\n";
        return 1;
    }

    int len = sizeof(cliaddr);
    while (true) {
        int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr, (socklen_t*)&len);
        if (n <= 0) {
            //Use this as an indicator that the sender has finished transmitting <=============
            break;
        }

        Packet packet;
        std::memcpy(&packet, buffer, sizeof(packet));
        
        file.seekp(packet.seq_num * DATA_SIZE);
        file.write(packet.data, n - HEADER_SIZE);

        // Send acknowledgment for received packet <=============
        int ack = packet.seq_num;
        sendto(sockfd, &ack, sizeof(ack), 0, (struct sockaddr*) &cliaddr, len);
    }

    file.close();
    close(sockfd);
    return 0;
}
