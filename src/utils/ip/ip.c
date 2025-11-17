#include "ft_ping.h"


#define TIMEOUT_SEC 1

void	*free_ip(void *ptr)
{
	t_ip	*cast = (t_ip *)ptr;

	if (!cast)
		return (NULL);
	if (cast->socket.socket > 0)
		close(cast->socket.socket);
	free(cast->ip);
	free(cast->solved);
	free(cast);
	return (NULL);
}

t_ip	*new_ip(char *ip)
{
	t_ip	*result;
	struct addrinfo hints;
	struct addrinfo *res = NULL;
	int err;

	result = malloc(sizeof(t_ip));
	if (!result)
		return (NULL);
	memset(result, 0, sizeof(t_ip));

	result->ip = strdup(ip);
	if (!result->ip)
		return (free_ip(result));

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;

	err = getaddrinfo(result->ip, NULL, &hints, &res);
	if (err != 0 || !res) {
		fprintf(stderr, "ft_ping: cannot resolve %s: %s\n", result->ip, gai_strerror(err));
		return (free_ip(result));
	}

	// Copy resolved address
	result->socket.dest_addr = *(struct sockaddr_in *)res->ai_addr;
	freeaddrinfo(res);

	// Create the raw ICMP socket
	result->socket.socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (result->socket.socket < 0) {
		write(2, "ft_ping: make sure you are running this as sudo\n", 48);
		return (free_ip(result));
	}


	// Configure socket timeout
	struct timeval tv = { .tv_sec = TIMEOUT_SEC, .tv_usec = 0 };
	if (setsockopt(result->socket.socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
		perror("ft_ping: error setting timeout");
		return (free_ip(result));
	}

	char resolved_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &result->socket.dest_addr.sin_addr, resolved_ip, sizeof(resolved_ip));

	result->solved = ft_strdup(resolved_ip);
	if (!result->solved)
	{
		perror("ft_ping: error in dup");
		return (free_ip(result));
	}

	return (result);
}