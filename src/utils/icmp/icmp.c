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
    char recvbuf[RECV_BUF];
    struct sockaddr_in from;
    socklen_t fromlen = sizeof(from);
    ssize_t recvd = recvfrom(
        ip->socket.socket, 
        recvbuf, 
        sizeof(recvbuf), 
        0,
        (struct sockaddr*)&from,
        &fromlen
    );

    if (recvd < 0) {
        return 1;
    }

    // parse IP header
    struct ip *ip_hdr = (struct ip *)recvbuf;
    int ip_hdr_len = ip_hdr->ip_hl * 4;

    // outer ICMP
    struct icmphdr *outer_icmp = (struct icmphdr *)(recvbuf + ip_hdr_len);

    if (outer_icmp->type == ICMP_TIME_EXCEEDED) {


        // router tells us TTL expired; inside is original IP + 8 bytes of original payload
        // inner IP begins after outer ICMP header
        char *inner = recvbuf + ip_hdr_len + sizeof(struct icmphdr);
        struct ip *inner_ip = (struct ip *)inner;
        int inner_ip_len = inner_ip->ip_hl * 4;
        struct icmphdr *inner_icmp = (struct icmphdr *)(inner + inner_ip_len);

        // check id/seq match
        if (ntohs(inner_icmp->un.echo.id) == ping->pid)
        {
            char addrstr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &from.sin_addr, addrstr, sizeof(addrstr));
            printf("%2d  %s  (type=11)\n", ping->ttl, addrstr);
        }
        else
        {
            // not our probe; ignore or handle
            printf("%2d  %s  (time-exceeded, not matching id)\n", ping->ttl, inet_ntoa(from.sin_addr));
        }


    } else if (outer_icmp->type == ICMP_ECHOREPLY) {



        // destination replied
        char addrstr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &from.sin_addr, addrstr, sizeof(addrstr));

        // compute RTT if you stored timestamp in payload
        struct timeval t2;
        gettimeofday(&t2, NULL);
        struct timeval tsent;
        memcpy(&tsent, recvbuf + ip_hdr_len + sizeof(struct icmphdr), sizeof(tsent));
        double rtt = (t2.tv_sec - tsent.tv_sec) * 1000.0 + (t2.tv_usec - tsent.tv_usec) / 1000.0;

        printf("%2d  %s  (echo-reply)  RTT=%.3f ms\n", ping->ttl, addrstr, rtt);
        return 1;


    } else {
        /*
        // other ICMP types (dest unreachable, etc)
        printf("%2d  %s  (ICMP type %d code %d)\n", ping->ttl,
                inet_ntoa(from.sin_addr), outer_icmp->type, outer_icmp->code);
        */
    }
    return 0;
//	printf("64 bytes from 108.157.93.36: icmp_seq=%d ttl=248 time=3,887 ms\n", i);
}