/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 08:18:02 by samusanc          #+#    #+#             */
/*   Updated: 2025/05/26 17:43:49 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

t_ping *ping = NULL;

void	print_round_trip(t_ip *ip)
{
	printf("round-trip min/avg/max/stddev = 0,102/0,113/0,126/0,000 ms\n");

}

void	print_ping_result(t_ip *ip)
{
	if (!ip)
		return ;

	printf("--- %s ping statistics ---\n", ip->ip);
	if (ip->packets_transmitted == 0) {
		printf("0 packets transmitted, 0 packets received, 100%% packet loss\n");
		return;
	}
	double loss = ((double)(ip->packets_transmitted - ip->packets_received)
					/ ip->packets_transmitted) * 100.0;
	printf("%zu packets transmitted, %zu packets received, %.0f%% packet loss\n",
		ip->packets_transmitted,
		ip->packets_received,
		loss);
	if (ip->packets_received)
		print_round_trip(ip);
}

int	ft_ping(t_node *new_ipv4)
{
	if (!new_ipv4)
		return 0;
	t_ip	*ip = (t_ip *)new_ipv4->content;
	double	min = 0.0f;
	double	max = 0.0f;
	double	total = 0.0f;
	size_t	i = 0;

	// what is printing here???
	printf("PING %s (%s) 56(84) bytes of data.\n", ip->ip, ip->solved);
	// this while 1 need to be changed to c
	while(1)
	{
		if (!ping->alive)
			break ;
		// update icmp package
		update_icmp(i, ip);
		// send icmp package
		send_icmp(i, ip);
		// recv icmp package
		recv_icmp(i, ip);
		sleep(1);
		i++;
	}
	// need a revision?
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
	//send_icmp();
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
