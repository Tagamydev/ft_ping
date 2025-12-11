#include "ft_ping.h"

uint16_t checksum(void *buf, size_t len) {
    uint32_t sum = 0;
    uint16_t *data = (uint16_t *)buf;
    
    while (len > 1) {
        sum += *data++;
        len -= 2;
    }
    
    if (len == 1) {
        uint16_t last = 0;
        *((uint8_t *)&last) = *(uint8_t *)data;
        sum += last;
    }
    
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    return (uint16_t)(~sum);
}

static void handle_time_exceeded(char *recvbuf, int ip_hdr_len, struct sockaddr_in *from) {
    char *inner = recvbuf + ip_hdr_len + sizeof(struct icmphdr);
    struct ip *inner_ip = (struct ip *)inner;
    int inner_ip_len = inner_ip->ip_hl * 4;
    
    struct icmphdr *inner_icmp = (struct icmphdr *)(inner + inner_ip_len);
    
    char addrstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &from->sin_addr, addrstr, sizeof(addrstr));
    
    if (ntohs(inner_icmp->un.echo.id) == ping->pid) {
        printf("%2d  %s  (TTL expired)\n", ping->ttl, addrstr);
    } else {
        printf("%2d  %s  (TTL expired, but not our packet - ignoring)\n", 
               ping->ttl, addrstr);
    }
}

static int handle_echo_reply(char *recvbuf, int ip_hdr_len, struct sockaddr_in *from, int seq, t_ip *ip, size_t recvd) {
    if (recvd > 0)
        recvd -= 20;

    char addrstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &from->sin_addr, addrstr, sizeof(addrstr));
    
    struct timeval t2;
    gettimeofday(&t2, NULL);
    
    struct icmphdr *icmp = (struct icmphdr *)(recvbuf + ip_hdr_len);
    int actual_seq = ntohs(icmp->un.echo.sequence);
    
    struct timeval tsent;
    memcpy(&tsent, recvbuf + ip_hdr_len + sizeof(struct icmphdr), sizeof(tsent));
    
    double rtt = (t2.tv_sec - tsent.tv_sec) * 1000.0 + 
                 (t2.tv_usec - tsent.tv_usec) / 1000.0;
    
    struct ip *ip_hdr = (struct ip *)recvbuf;
    int received_ttl = ip_hdr->ip_ttl;
    int rtt_int = (int)rtt;
    int rtt_frac = (int)((rtt - rtt_int) * 1000);
    
    printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%d.%03d ms",
           recvd, addrstr, actual_seq, received_ttl, rtt_int, rtt_frac);    

    if (actual_seq == ip->last)
    {
        ip->dups += 1;
        printf(" (DUP!)\n");
    }
    else
    {
        printf("\n");
        ip->last = actual_seq;
    }

    ip->sum_rtt += rtt;
    ip->sum_rtt_square += rtt * rtt;

    if (rtt < ip->min)
        ip->min = rtt;
    if (rtt > ip->max)
        ip->max = rtt;
    ip->avg = ( ip->packets_received * ip->avg + rtt) / (ip->packets_received + 1);
    ip->packets_received += 1;

    return 1;
}


int update_icmp(int seq, t_ip *ip) {
    if (ping->ttl > 0) {
        if (setsockopt(ip->socket.socket, IPPROTO_IP, IP_TTL, 
                       &ping->ttl, sizeof(ping->ttl)) < 0) {
            perror("setsockopt(IP_TTL)");
            return 1;
        }
    }
    
    struct icmphdr *icmp = (struct icmphdr *)ping->icmp;
    
    icmp->un.echo.sequence = htons(seq);
    
    struct timeval t1;
    gettimeofday(&t1, NULL);
    memcpy(ping->icmp + sizeof(struct icmphdr), &t1, sizeof(t1));
    
    icmp->checksum = 0;
    icmp->checksum = checksum((void*)icmp, PACKET_SIZE);
    
    return 0;
}

int send_icmp(int seq, t_ip *ip) {
    ssize_t sent = sendto(
        ip->socket.socket, 
        ping->icmp, 
        PACKET_SIZE, 
        0,
        (struct sockaddr*)&ip->socket.dest_addr, 
        sizeof(ip->socket.dest_addr)
    );
    
    ip->packets_transmitted += 1;
    return 0;
}

int recv_icmp(int seq, t_ip *ip) {
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
        return -1;
    }
    
    struct ip *ip_hdr = (struct ip *)recvbuf;
    int ip_hdr_len = ip_hdr->ip_hl * 4;
    
    struct icmphdr *icmp = (struct icmphdr *)(recvbuf + ip_hdr_len);
    if (icmp->type == ICMP_TIME_EXCEEDED) {
        handle_time_exceeded(recvbuf, ip_hdr_len, &from);
        return 0;
    } 
    else if (icmp->type == ICMP_ECHOREPLY) {
        return handle_echo_reply(recvbuf, ip_hdr_len, &from, seq, ip, recvd);
    }
    return 0;
}
