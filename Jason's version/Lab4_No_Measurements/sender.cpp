#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

const int PORT = 8080;
const char* SERVER_IP = "192.168.20.2";  // Adjust this as needed <=============
const int MTU = 1500;  // Use 9000 for the other case <=============
const int HEADER_SIZE = sizeof(int);
const int DATA_SIZE = MTU - HEADER_SIZE;

struct Packet {
    int seq_num;
    char data[DATA_SIZE];
};

void send_file_thread(const char* filename, int start, int end) {
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    std::memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file.\n";
        return;
    }

    file.seekg(start * DATA_SIZE);

    for (int i = start; i < end && file.good(); ++i) {
        Packet packet;
        packet.seq_num = i;
        file.read(packet.data, DATA_SIZE);

        sendto(sockfd, &packet, sizeof(packet), 0,
               (const struct sockaddr*) &servaddr, sizeof(servaddr));
    }

    file.close();
    close(sockfd);
}

int main() {
    const char* filename = "data.bin";
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open file.\n";
        return 1;
    }

    int total_packets = file.tellg() / DATA_SIZE;
    file.close();

    int threads_count = 4;  // Thread number <=============
    int packets_per_thread = total_packets / threads_count;

    std::vector<std::thread> threads;
    for (int i = 0; i < threads_count; ++i) {
        int start = i * packets_per_thread;
        int end = (i == threads_count - 1) ? total_packets : start + packets_per_thread;
        threads.emplace_back(send_file_thread, filename, start, end);
    }

    for (auto& th : threads) {
        th.join();
    }

    return 0;
}
