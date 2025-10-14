#include "ft_ping.h"
#include "s_flags.h"

// ping destructor
void	free_ping()
{
    free(ping->icmp);
	free(ping);
}

// ping constructor
t_ping	*init_ping(char **argv)
{
	t_ping	*result = NULL;
	int		i = 0;
	
	result = malloc(sizeof(t_ping));
	if (!result)
		return NULL;
	bzero(result, sizeof(t_ping));
	parse_flags(argv, result);
	if (result->flags.error && result->flags.error != -1)
	{
		free(result);
		return NULL;
	}
	if (result->flags.error == -1)
	{
		print_help();
		return NULL;
	}
	while(argv[i])
	{
		i++;
	}

	result->pid = getpid() & 0xFFFF;
	result->alive = 1;


	return (result);
}