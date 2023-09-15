#include <iostream>
#include <fstream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

const int PORT = 8080;
const int MTU = 1500;  // Use 9000 for the other case <=============
const int HEADER_SIZE = sizeof(int);
const int DATA_SIZE = MTU - HEADER_SIZE;

struct Packet {
    int seq_num;
    char data[DATA_SIZE];
};

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    std::memset(&servaddr, 0, sizeof(servaddr));
    std::memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (const struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    std::ofstream file("received_data.bin", std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file.\n";
        return 1;
    }

    int len = sizeof(cliaddr);
    while (true) {
        Packet packet;
        int n = recvfrom(sockfd, &packet, sizeof(packet), 0, (struct sockaddr*) &cliaddr, (socklen_t*) &len);
        if (n <= 0) break;  // End of transmission <=============

        file.seekp(packet.seq_num * DATA_SIZE);
        file.write(packet.data, DATA_SIZE);
    }

    file.close();
    close(sockfd);
    return 0;
}
