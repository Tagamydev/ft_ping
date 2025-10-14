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

#ifndef S_PING_H
# define S_PING_H
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
//#include <strings.h>
//#include <stdint.h>
//#include <stddef.h>
# include <netinet/ip_icmp.h>
# include "ctype.h"

# include "s_flags.h"
typedef struct s_ping {
	t_flags		flags;
	t_list		ips;
	int			alive;
	int			error;
	size_t		ttl;
	pid_t		pid;
	char		*icmp;
} t_ping;

// global variable
extern t_ping	*ping;

void	free_ping();
t_ping	*init_ping(char **argv);

#endif