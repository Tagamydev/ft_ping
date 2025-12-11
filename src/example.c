// compile with: gcc -o mytrace mytrace.c
// run as root

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>
#include <errno.h>

#define MAXHOPS 30
#define PACKET_SIZE 64
#define RECV_BUF 1500
#define TIMEOUT_SEC 1

uint16_t icmp_checksum(void *buf, int len);

int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "usage: %s <dest-ip>\n", argv[0]); exit(1); }
    const char *dst = argv[1];
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    inet_pton(AF_INET, dst, &dest_addr.sin_addr);

    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0) { perror("socket"); exit(1); }

    struct timeval tv = { .tv_sec = TIMEOUT_SEC, .tv_usec = 0 };
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    pid_t pid = getpid() & 0xFFFF;
    uint16_t seq = 0;

    for (int ttl = 1; ttl <= MAXHOPS; ++ttl) {
        if (setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
            perror("setsockopt(IP_TTL)");
            break;
        }

        char sendbuf[PACKET_SIZE];
        memset(sendbuf, 0, PACKET_SIZE);
        struct icmphdr *icmp = (struct icmphdr *)sendbuf;
        icmp->type = ICMP_ECHO;
        icmp->code = 0;
        icmp->un.echo.id = htons(pid);

        icmp->un.echo.sequence = htons(seq++);
        struct timeval t1;
        gettimeofday(&t1, NULL);
        memcpy(sendbuf + sizeof(struct icmphdr), &t1, sizeof(t1));
        icmp->checksum = 0;
        icmp->checksum = icmp_checksum((void*)icmp, PACKET_SIZE);

        ssize_t sent = sendto(sock, sendbuf, PACKET_SIZE, 0,
                              (struct sockaddr*)&dest_addr, sizeof(dest_addr));
        if (sent < 0) { perror("sendto"); continue; }

        char recvbuf[RECV_BUF];
        struct sockaddr_in from;
        socklen_t fromlen = sizeof(from);
        ssize_t recvd = recvfrom(sock, recvbuf, sizeof(recvbuf), 0,
                                 (struct sockaddr*)&from, &fromlen);
        if (recvd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("%2d  *\n", ttl);
                continue;
            } else {
                perror("recvfrom");
                break;
            }
        }

        // parse IP header
        struct ip *ip_hdr = (struct ip *)recvbuf;
        int ip_hdr_len = ip_hdr->ip_hl * 4;

        // outer ICMP
        struct icmphdr *outer_icmp = (struct icmphdr *)(recvbuf + ip_hdr_len);

        if (outer_icmp->type == ICMP_TIME_EXCEEDED) {
            char *inner = recvbuf + ip_hdr_len + sizeof(struct icmphdr);
            struct ip *inner_ip = (struct ip *)inner;
            int inner_ip_len = inner_ip->ip_hl * 4;
            struct icmphdr *inner_icmp = (struct icmphdr *)(inner + inner_ip_len);

            if (ntohs(inner_icmp->un.echo.id) == pid) {
                char addrstr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &from.sin_addr, addrstr, sizeof(addrstr));
                printf("%2d  %s  (type=11)\n", ttl, addrstr);
            } else {
                printf("%2d  %s  (time-exceeded, not matching id)\n", ttl, inet_ntoa(from.sin_addr));
            }
        } else if (outer_icmp->type == ICMP_ECHOREPLY) {
            char addrstr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &from.sin_addr, addrstr, sizeof(addrstr));

            struct timeval t2;
            gettimeofday(&t2, NULL);
            struct timeval tsent;
            memcpy(&tsent, recvbuf + ip_hdr_len + sizeof(struct icmphdr), sizeof(tsent));
            double rtt = (t2.tv_sec - tsent.tv_sec) * 1000.0 + (t2.tv_usec - tsent.tv_usec) / 1000.0;

            printf("%2d  %s  (echo-reply)  RTT=%.3f ms\n", ttl, addrstr, rtt);
            break;
        } else {
            printf("%2d  %s  (ICMP type %d code %d)\n", ttl,
                   inet_ntoa(from.sin_addr), outer_icmp->type, outer_icmp->code);
        }
    }

    close(sock);
    return 0;
}

uint16_t icmp_checksum(void *buf, int len) {
    uint32_t sum = 0;
    uint16_t *data = buf;
    while (len > 1) {
        sum += *data++;
        len -= 2;
    }
    if (len == 1) {
        uint16_t last = 0;
        *((uint8_t *)&last) = *((uint8_t*)data);
        sum += last;
    }
    while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
    return (uint16_t)(~sum);
}
