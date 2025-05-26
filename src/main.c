/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 08:18:02 by samusanc          #+#    #+#             */
/*   Updated: 2025/05/26 14:42:35 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"
#include "libft.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>

typedef struct s_ip{
	char		*ip;
	size_t		packets_transmitted;
	size_t		packets_received;
	size_t		time;
	clock_t		start_time;
	clock_t		end_time;
	double		min;
	double		avg;
	double		max;
} t_ip;

typedef struct s_ping{
	t_flags		flags;
	t_list		ips;
	int			alive;
	int			error;
} t_ping;

t_ping	*ping = NULL;

void	*free_ip(void *ptr)
{
	t_ip	*cast;

	cast = (t_ip *)ptr;
	free(cast->ip);
	free(ptr);
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
	return (result);
}

t_ping	*init_ping(char **argv)
{
	t_ping	*result = NULL;
	int		i = 0;

	result = malloc(sizeof(t_ping));
	if (!result)
		return NULL;
	bzero(result, sizeof(t_ping));
	while(argv[i])
	{
		if (argv[i][0] != '-')
			list_push_b(&result->ips, node(new_ip(argv[i]), free_ip));
		i++;
	}
	result->alive = 1;
	return (result);
}

void	print_ping_result(t_ip *ip)
{
	printf("--- 172.0.0.21 ping statistics ---\n");
	printf("4 packets transmitted, 0 packets received, 100%% packet loss\n");
	if (!ip->packets_received)
		printf("round-trip min/avg/max/stddev = 0,102/0,113/0,126/0,000 ms\n");
}

int	ft_ping(t_node *new_ipv4)
{
	if (!new_ipv4)
		return 0;
	t_ip	*ip = (t_ip *)new_ipv4->content;
	double	min = 0.0f;
	double	max = 0.0f;
	double	total = 0.0f;

	printf("PING %s (%s) 56(84) bytes of data.\n", ip->ip, ip->ip);
	while(1)
	{
		if (!ping->alive)
			return 0;
		printf("64 bytes from 108.157.93.36: icmp_seq=1 ttl=248 time=3,887 ms\n");
		sleep(1);
	}
	print_ping_result(ip);
	return 0;
}

void handle_signal(int sig)
{
	if (!ping)
		exit(1);
    if (sig == SIGINT || sig == SIGTERM)
		ping->alive = 0;
}

int	main(int argc, char **argv)
{
	argv++;
	int		error;
    struct	sigaction sa;
	t_flags	flags = parse_flags(argv);
	
	if (flags.error && flags.error != -1)
		return 1;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

	ping = init_ping(argv);
	t_node	*tmp = ping->ips.head;
	for (size_t i = 0; i < ping->ips.size; i++) {
		ft_ping(tmp);
		tmp = tmp->next;
	}

	list_clear(&ping->ips);
	error = ping->error;
	free(ping);
	return (error);
}
