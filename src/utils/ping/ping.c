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

t_ping	*init_ping(char **argv)
{
	t_ping	*result;
	int		i = 0;

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

	while (argv[i])
		i++;

	result->pid = getpid() & 0xFFFF;
	result->alive = 1;
	result->icmp = NULL;

	return (result);
}
