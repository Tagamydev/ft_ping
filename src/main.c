/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 08:18:02 by samusanc          #+#    #+#             */
/*   Updated: 2025/05/26 09:59:17 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"
#include "libft.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

typedef struct s_ping{
	t_flags		flags;
	char		*ip;
	size_t		packets_transmitted;
	size_t		packets_received;
	size_t		time;
	clock_t		start_time;
	clock_t		end_time;
} t_ping;

t_ping	*ping = NULL;

t_ping	*init_ping(char *ip)
{
	t_ping	*result = NULL;

	result = malloc(sizeof(t_ping));
	if (!result)
		return NULL;
	bzero(result, sizeof(t_ping));
	result->ip = ip;
	result->packets_transmitted = 3;
	result->packets_received = 2;
	result->start_time = clock();
	return result;
}

int	ft_ping(char *file, t_flags flags, int multiple)
{
	if (file[0] == '-')
		return 0;
	int	i = 1;
	printf("PING %s (%s) 56(84) bytes of data.\n", ping->ip, ping->ip);

	while(1)
	{
		printf("From 192.168.0.182 icmp_seq=%d Destination Host Unreachable\n", i++);
		sleep(1);
	}
	return 0;
}

void handle_signal(int sig)
{
	printf("\n");

	if (!ping)
		exit(1);
	ping->end_time = clock();
    if (sig == SIGINT || sig == SIGTERM)
	{
		float	percentage = (float)(ping->packets_received + 1) / (float)(ping->packets_transmitted + 1);
		double cpu_time_used = ((double) (ping->end_time - ping->start_time)) / CLOCKS_PER_SEC;
		percentage *= 100;
		printf("--- %s ping statistics ---\n", ping->ip);
		printf("%zu packets transmitted, %zu received, %zu%% packet loss, time %zums\n", ping->packets_transmitted, ping->packets_received, 
				(size_t)percentage, (size_t)(cpu_time_used * 1000.0));
	}
    exit(0);
}

int	main(int argc, char **argv)
{
	int		error_counter = 0;
    struct	sigaction sa;
	t_flags	flags = parse_flags(argv);
	
	if (flags.error && flags.error != -1)
	{
		print_help();
		return 1;
	}

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

	ping = init_ping("192.0.0.1");
	ft_ping(argv[0], flags, 1);
	return 1;
}
