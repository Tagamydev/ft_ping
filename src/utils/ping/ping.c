#include "ft_ping.h"
#include "s_flags.h"

void	free_ping(void)
{
	if (!ping)
		return;
	free(ping->icmp);
	ping->icmp = NULL;
	free(ping);
	ping = NULL;
}

void    config_icmp(char *ptr, pid_t pid)
{
    memset(ptr, 0, PACKET_SIZE);
    struct icmphdr *icmp = (struct icmphdr *)ptr;
    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->un.echo.id = htons(pid);
}

t_ping	*init_ping(char **argv)
{
	t_ping	*result;

	result = malloc(sizeof(t_ping));
	if (!result)
		return (NULL);

	memset(result, 0, sizeof(t_ping));
	if (!parse_flags(argv, result))
	{
        printf("ft_ping: error parsing flags\n");
		free(result);
		return (NULL);
	}

	if (result->flags.error && result->flags.error != -1)
	{
		free(result);
		return (NULL);
	}
	if (result->flags.error == -1)
	{
		print_help();
		free(result);
		return (NULL);
	}

	result->pid = getpid() & 0xFFFF;
	result->alive = 1;
    result->ttl = 30;

    char sendbuf[PACKET_SIZE];

    result->icmp = malloc(sizeof(sendbuf));
    if (!result->icmp)
	{
		free(result);
		return (NULL);
	}
    config_icmp(result->icmp, result->pid);
	return (result);
}
