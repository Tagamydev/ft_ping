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

#ifndef FT_PING_H
# define FT_PING_H
# include "s_flags.h"
# include "s_ip.h"
# include "s_ping.h"
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
# include <signal.h>
# include <netdb.h>          // <-- required for struct addrinfo, getaddrinfo(), freeaddrinfo(), gai_strerror()

//#include <strings.h>
//#include <stdint.h>
//#include <stddef.h>
# include <netinet/ip_icmp.h>
# include <sys/time.h>

typedef	struct s_header{
	size_t	addr;
	char	type_char;
	char	*name;
}	t_header;


#define TIMEOUT_SEC 1
#define PACKET_SIZE 64
#define RECV_BUF 1500

void	print_help();
int		error(char *title, char *message, int i);


void	*free_header(void *ptr);
char	*convert_addr(int addr, int is_64);


// ICMP FT
int update_icmp(int i, t_ip *ip);
int send_icmp(int i, t_ip *ip);
int recv_icmp(int i, t_ip *ip);

//t_ping	*ping = NULL;


#endif
