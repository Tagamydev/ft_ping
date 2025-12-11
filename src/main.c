/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 08:18:02 by samusanc          #+#    #+#             */
/*   Updated: 2025/12/11 20:00:00 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"
#include <math.h>
#include <sys/time.h>

t_ping *ping = NULL;

void	print_round_trip(t_ip *ip)
{
	double variance = ((double)ip->sum_rtt_square / (double)ip->packets_received) - (double)(ip->avg * ip->avg);
	variance = sqrt(variance);
	printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", ip->min, ip->avg, ip->max, variance);
}

void	print_ping_result(t_ip *ip)
{
	if (!ip)
		return ;

	printf("--- %s ping statistics ---\n", ip->ip);
	if (ip->packets_transmitted == 0) {
		printf("1 packets transmitted, 0 packets received, 100%% packet loss\n");
		return;
	}
	double loss = 0;
	int	dup = 0;
	int pr = (int)ip->packets_received;
	int pt = (int)ip->packets_transmitted;
	int total = pt - pr;

	if (total >= 0)
		loss = ((double)(ip->packets_transmitted - ip->packets_received) / ip->packets_transmitted) * 100.0;
	else
	{
		if (ip->packets_received)
			loss = ((double)(ip->packets_received - ip->packets_transmitted) / ip->packets_received) * 100.0;
		else
			loss = 100;

		dup = -total;
	}

	printf("%zu packets transmitted, %zu packets received", ip->packets_transmitted, ip->packets_received);
	if (!(total >= 0))
		printf(", +%d duplicates", ip->dups);
	printf(", %.0f%% packet loss\n", loss);
	if (ip->packets_received)
		print_round_trip(ip);
}

static double get_time_seconds(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + tv.tv_usec / 1000000.0;
}

static void wait_while_recv(int i, t_ip *ip, double interval_start, double super_start)
{
	double current_time;
	double elapsed_from_start;
	double elapsed_from_interval;
	double packet_timeout = ping->flags.w ? ping->flags.timeout : ping->flags.interval;
	double timeout_deadline = interval_start + packet_timeout;

	do {
		recv_icmp(i, ip);
		current_time = get_time_seconds();
		
		if (ping->flags.W)
		{
			elapsed_from_start = current_time - super_start;
			if (elapsed_from_start >= (double)ping->flags.deadline)
			{
				ping->alive = 0;
				return;
			}
		}
		
		if (ping->flags.w && current_time >= timeout_deadline)
		{
			break;
		}
		elapsed_from_interval = current_time - interval_start;
		
	} while (elapsed_from_interval < ping->flags.interval);
}

static void sleep_remaining_interval(double interval_start)
{
	double current_time = get_time_seconds();
	double elapsed = current_time - interval_start;
	double remaining = ping->flags.interval - elapsed;
	
	if (remaining > 0)
	{
		usleep((useconds_t)(remaining * 1000000));
	}
}

int	ft_ping(t_node *new_ipv4)
{
	if (!new_ipv4)
		return 0;
	
	t_ip	*ip = (t_ip *)new_ipv4->content;
	size_t	i = 0;
	double	super_start = get_time_seconds();

	printf("PING %s (%s): 56 data bytes", ip->ip, ip->solved);
	if (ping->flags.v)
	{
		uint16_t id = getpid() & 0xFFFF;
		printf(", id = 0x%04x = %d", id, id);
	}
	printf("\n");
	
	while(1)
	{
		if (!ping->alive)
			break;
		
		double interval_start = get_time_seconds();
		update_icmp(i, ip);
		send_icmp(i, ip);
		wait_while_recv(i, ip, interval_start, super_start);
		
		if (!ping->alive)
			break;
		
		i++;
		
		// Check if we've reached the packet count limit (-c flag)
		if (ping->flags.c && i >= ping->flags.number)
			break;
		
		sleep_remaining_interval(interval_start);
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
	if (argc == 1)
	{
		write(2, "ping: missing host operand\nTry 'ping --help' or 'ping --usage' for more information.\n", 85);
		return (64);
	}

	argv++;
	int		error;
	struct	sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handle_signal;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);

	ping = init_ping(argv);
	if (!ping)
		return (-1);
	
	t_node	*tmp = ping->ips.head;
	for (size_t i = 0; i < ping->ips.size; i++) {
		ft_ping(tmp);
		tmp = tmp->next;
	}

	list_clear(&ping->ips);
	error = ping->error;
	free_ping(ping);
	return (error);
}
