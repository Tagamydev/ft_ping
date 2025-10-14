#include "ft_ping.h"

void	*free_ip(void *ptr)
{
	t_ip	*cast;

	cast = (t_ip *)ptr;
	free(cast->ip);
	free(ptr);
	close(cast->socket.socket);
	return (NULL);
}

t_ip	*new_ip(char *ip)
{
	t_ip	*result;

	result = malloc(sizeof(t_ip));
	if (!result)
		return (result);
	bzero(result, sizeof(t_ip));
	result->ip = strdup(ip);

	// start socket

    //if (argc != 2) { fprintf(stderr, "usage: %s <dest-ip>\n", argv[0]); exit(1); }
    memset(&result->socket.dest_addr, 0, sizeof(result->socket.dest_addr));
    result->socket.dest_addr.sin_family = AF_INET;
    inet_pton(AF_INET, result->ip, &result->socket.dest_addr.sin_addr);
    result->socket.socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (result->socket.socket < 0)
	{
		free_ip(result);
		return (NULL);
	}


    struct timeval tv = { .tv_sec = TIMEOUT_SEC, .tv_usec = 0 };
	if (setsockopt(result->socket.socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
	{
		free_ip(result);
		return (NULL);
	}

	if (setsockopt(result->socket.socket, IPPROTO_IP, IP_TTL, &ping->ttl, sizeof(ping->ttl)) < 0)
	{
		free_ip(result);
		return (NULL);
	}

	return (result);
}