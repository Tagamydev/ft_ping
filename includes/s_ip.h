/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 08:54:32 by samusanc          #+#    #+#             */
/*   Updated: 2025/05/26 17:36:04 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef S_IP_H
# define S_IP_H
# include <stdio.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/mman.h>
# include <stdlib.h>
# include <string.h>
# include <elf.h>
# include <time.h>
# include "libft.h"
#include <signal.h>
//#include <strings.h>
//#include <stdint.h>
//#include <stddef.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

typedef struct s_socket {
	int			socket;
    struct sockaddr_in dest_addr;
} t_socket;

typedef struct s_ip {
	char		*ip;
	char		*solved;
	size_t		packets_transmitted;
	size_t		packets_received;
	size_t		time;
	clock_t		start_time;
	clock_t		end_time;
	double		min;
	double		avg;
	double		max;
	t_socket	socket;
	int			last;
	double		sum_rtt;
	double		sum_rtt_square;
	size_t		dups;

} t_ip;

void	*free_ip(void *ptr);
t_ip	*new_ip(char *ip);

#endif