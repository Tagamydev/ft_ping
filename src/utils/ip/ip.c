#include "ft_ping.h"

void	*free_ip(void *ptr)
{
	t_ip	*cast = (t_ip *)ptr;

	if (!cast)
		return (NULL);
	if (cast->socket.socket > 0)
		close(cast->socket.socket);
	free(cast->ip);
	free(cast);
	return (NULL);
}

t_ip	*new_ip(char *ip)
{
	t_ip	*result;

	result = malloc(sizeof(t_ip));
	if (!result)
		return (NULL);
	memset(result, 0, sizeof(t_ip));
	result->ip = strdup(ip);
	if (!result->ip)
		return (free_ip(result));

	memset(&result->socket.dest_addr, 0, sizeof(result->socket.dest_addr));
	result->socket.dest_addr.sin_family = AF_INET;

	if (inet_pton(AF_INET, result->ip, &result->socket.dest_addr.sin_addr) <= 0)
		return (free_ip(result));

	result->socket.socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (result->socket.socket < 0)
    {
        write(2, "ft_ping: make sure you are running this as sudo\n", 48);
		return (free_ip(result));
    }

	struct timeval tv = { .tv_sec = TIMEOUT_SEC, .tv_usec = 0 };
	if (setsockopt(result->socket.socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        printf("ft_ping: error timeval\n");
		return (free_ip(result));
    }

    /*
	if (setsockopt(result->socket.socket, IPPROTO_IP, IP_TTL, &ping->ttl, sizeof(ping->ttl)) < 0)
    {
        printf("ft_ping: error second socket opt\n");
		return (free_ip(result));
    }
    */
    printf("new ip added: %s\n", ip);

	return (result);
}