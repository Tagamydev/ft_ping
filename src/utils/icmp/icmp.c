#include "ft_ping.h"

// what is this for??????
uint16_t checksum(void *buf, size_t len) {
    uint32_t sum = 0;
    uint16_t *data = (uint16_t *)buf;
    while (len > 1) {
        sum += *data++;
        len -= 2;
    }
    if (len == 1) {             // odd byte
        uint16_t last = 0;
        *((uint8_t *)&last) = *(uint8_t *)data;
        sum += last;
    }
    // fold 32-bit sum to 16 bits
    while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
    return (uint16_t)(~sum);
}


int update_icmp(int i, t_ip *ip)
{
    if (setsockopt(ip->socket.socket, IPPROTO_IP, IP_TTL, &ping->ttl, sizeof(ping->ttl)) < 0) {
        // fix this
        perror("setsockopt(IP_TTL)");
        return 1;
    }

    struct icmphdr *icmp = (struct icmphdr *)ping->icmp;
    struct timeval t1;

    icmp->un.echo.sequence = htons(i);
    gettimeofday(&t1, NULL);
    memcpy(ping->icmp + sizeof(struct icmphdr), &t1, sizeof(t1));
    icmp->checksum = 0;
    icmp->checksum = checksum((void*)icmp, PACKET_SIZE);
}

// send icmp need to be rework!!!!
int send_icmp(int i, t_ip *ip)
{
    ssize_t sent = sendto(
        ip->socket.socket, 
        ping->icmp, 
        PACKET_SIZE, 
        0,
        (struct sockaddr*)&ip->socket.dest_addr, 
        sizeof(ip->socket.dest_addr)
    );
    if (sent < 0)
        return -1;
	/*
	int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	struct sockaddr_in dst = { .sin_family = AF_INET, .sin_addr.s_addr = inet_addr("127.0.0.1") };
	char packet[64];
	struct icmphdr *icmp = (struct icmphdr*)packet;
	icmp->type = ICMP_ECHO;
	icmp->code = 0;
	icmp->un.echo.id = htons(getpid() & 0xFFFF);
	icmp->un.echo.sequence = htons(1);
	// payload: fill rest with pattern + timestamp if desired
	icmp->checksum = 0;
	icmp->checksum = checksum(packet, sizeof(packet));
	sendto(sock, packet, sizeof(packet), 0, (struct sockaddr*)&dst, sizeof(dst));
	return (0);
	*/
}

int recv_icmp(int i, t_ip *ip)
{
	printf("64 bytes from 108.157.93.36: icmp_seq=%d ttl=248 time=3,887 ms\n", i);
}