#include "ft_ping.h"

uint16_t checksum(void *buf, size_t len) {
    uint32_t sum = 0;
    uint16_t *data = (uint16_t *)buf;
    
    // Sum all 16-bit words
    while (len > 1) {
        sum += *data++;
        len -= 2;
    }
    
    // Handle odd byte (if packet length is odd)
    if (len == 1) {
        uint16_t last = 0;
        *((uint8_t *)&last) = *(uint8_t *)data;
        sum += last;
    }
    
    // Fold 32-bit sum into 16 bits (add carry bits back)
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    return (uint16_t)(~sum);
}

/**
 * Handle ICMP Time Exceeded (Type 11) response
 * =============================================
 * When a router receives a packet with TTL=0, it sends back an ICMP Time Exceeded message.
 * This message contains:
 *   - The router's IP address (who dropped the packet)
 *   - The original IP header + first 8 bytes of original packet (so we can identify it)
 * 
 * We need to:
 *   1. Extract the inner (original) ICMP packet from the error message
 *   2. Verify it's our packet by checking the ICMP ID
 *   3. Print which router responded
 */
static void handle_time_exceeded(char *recvbuf, int ip_hdr_len, struct sockaddr_in *from) {
    // Skip: [outer IP header][outer ICMP header] to reach the embedded original packet
    char *inner = recvbuf + ip_hdr_len + sizeof(struct icmphdr);
    struct ip *inner_ip = (struct ip *)inner;
    int inner_ip_len = inner_ip->ip_hl * 4;
    
    // Now we're at the original ICMP header that we sent
    struct icmphdr *inner_icmp = (struct icmphdr *)(inner + inner_ip_len);
    
    char addrstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &from->sin_addr, addrstr, sizeof(addrstr));
    
    // Verify this is our packet by checking the ICMP ID (we set it to our PID)
    if (ntohs(inner_icmp->un.echo.id) == ping->pid) {
        printf("%2d  %s  (TTL expired)\n", ping->ttl, addrstr);
    } else {
        // This is someone else's packet, ignore it
        printf("%2d  %s  (TTL expired, but not our packet - ignoring)\n", 
               ping->ttl, addrstr);
    }
}

/**
 * Handle ICMP Echo Reply (Type 0) response
 * =========================================
 * This is the normal "pong" response from the destination host.
 * We extract:
 *   1. The timestamp we embedded in the packet
 *   2. Calculate round-trip time (RTT)
 *   3. Extract the TTL from the IP header of the received packet
 *   4. Display the result
 */
static int handle_echo_reply(char *recvbuf, int ip_hdr_len, struct sockaddr_in *from, int seq, t_ip *ip, size_t recvd) {
    // e
    if (recvd > 0)
        recvd -= 20;

    // this i think is wrong maybe need rework because i dont read the data from the response, i think, the response have time??? idk
    char addrstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &from->sin_addr, addrstr, sizeof(addrstr));
    
    struct timeval t2;
    gettimeofday(&t2, NULL);
    
    // Get the actual sequence number from the received packet
    struct icmphdr *icmp = (struct icmphdr *)(recvbuf + ip_hdr_len);
    int actual_seq = ntohs(icmp->un.echo.sequence);  // ← Use THIS, not the parameter!
    
    struct timeval tsent;
    memcpy(&tsent, recvbuf + ip_hdr_len + sizeof(struct icmphdr), sizeof(tsent));
    
    double rtt = (t2.tv_sec - tsent.tv_sec) * 1000.0 + 
                 (t2.tv_usec - tsent.tv_usec) / 1000.0;
    
    struct ip *ip_hdr = (struct ip *)recvbuf;
    int received_ttl = ip_hdr->ip_ttl;
    int rtt_int = (int)rtt;
    int rtt_frac = (int)((rtt - rtt_int) * 1000);
    
    // Use actual_seq from the packet, not the parameter
    printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%d.%03d ms",
           recvd, addrstr, actual_seq, received_ttl, rtt_int, rtt_frac);    

    if (actual_seq != seq)
        printf(" (DUP!)\n");
    else
        printf("\n");

    if (rtt < ip->min)
        ip->min = rtt;
    if (rtt > ip->max)
        ip->max = rtt;
    ip->avg = ( ip->packets_received * ip->avg + rtt) / (ip->packets_received + 1);
    ip->packets_received += 1;
    return 1;
}

/**
 * Update ICMP packet for sending
 * ===============================
 * Prepares an ICMP Echo Request packet:
 *   1. Set socket TTL option (Time To Live) - ONLY if manually configured
 *   2. Set sequence number
 *   3. Embed current timestamp in payload (for RTT calculation)
 *   4. Calculate and set checksum
 */
int update_icmp(int seq, t_ip *ip) {
    // Set TTL at the IP level ONLY if user configured it manually
    // If ping->ttl is 0 or negative, use system default TTL (usually 64)
    if (ping->ttl > 0) {
        if (setsockopt(ip->socket.socket, IPPROTO_IP, IP_TTL, 
                       &ping->ttl, sizeof(ping->ttl)) < 0) {
            perror("setsockopt(IP_TTL)");
            return 1;
        }
    }
    
    struct icmphdr *icmp = (struct icmphdr *)ping->icmp;
    
    // Set sequence number (in network byte order)
    icmp->un.echo.sequence = htons(seq);
    
    // Embed current timestamp in the ICMP payload
    // This allows us to calculate RTT when we receive the reply
    struct timeval t1;
    gettimeofday(&t1, NULL);
    memcpy(ping->icmp + sizeof(struct icmphdr), &t1, sizeof(t1));
    
    // Calculate checksum (must zero it first, then compute)
    icmp->checksum = 0;
    icmp->checksum = checksum((void*)icmp, PACKET_SIZE);
    
    return 0;
}

/**
 * Send ICMP Echo Request packet
 * ==============================
 * Transmits the prepared ICMP packet to the destination
 */
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

/**
 * Receive and process ICMP response
 * ==================================
 * Waits for an ICMP response and handles two main types:
 *   1. ICMP_TIME_EXCEEDED (Type 11): A router along the path dropped our packet
 *   2. ICMP_ECHOREPLY (Type 0): The destination host replied
 * 
 * Returns:
 *   1 if echo reply received (destination reached)
 *   0 if time exceeded or other non-terminal response
 *   -1 on error
 */
int recv_icmp(int seq, t_ip *ip) {
    char recvbuf[RECV_BUF];
    struct sockaddr_in from;
    socklen_t fromlen = sizeof(from);
    
    // Receive response
    ssize_t recvd = recvfrom(
        ip->socket.socket, 
        recvbuf, 
        sizeof(recvbuf), 
        0,
        (struct sockaddr*)&from,
        &fromlen
    );
    
    if (recvd < 0) {
       // perror("recvfrom");
        return -1;
    }
    
    // Parse IP header to find where ICMP data starts
    struct ip *ip_hdr = (struct ip *)recvbuf;
    int ip_hdr_len = ip_hdr->ip_hl * 4; // IP header length in bytes
    
    // Get ICMP header
    struct icmphdr *icmp = (struct icmphdr *)(recvbuf + ip_hdr_len);
    
    // Handle different ICMP message types
    if (icmp->type == ICMP_TIME_EXCEEDED) {
        // A router told us the packet's TTL expired
        handle_time_exceeded(recvbuf, ip_hdr_len, &from);
        return 0;
    } 
    else if (icmp->type == ICMP_ECHOREPLY) {
        // The destination replied - we've reached it!
        return handle_echo_reply(recvbuf, ip_hdr_len, &from, seq, ip, recvd);
    }
    return 0;
}
