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

#ifndef S_FLAGS_H
# define S_FLAGS_H
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

typedef struct s_flags{

	int		c;
	size_t	number;
	int		w;
	size_t	deadline;
	int W;
	int	timeout;
	int	p;
	char	*pad;
	int q;
	int i;
	float	interval;
	int v;
	int help;
	int	total;
	int	error;
} t_flags;

# include "s_ping.h"

int		parse_flag(int *value_flag, char *last_flag, char *flag);
void	*parse_flags(char **argv, t_ping *result);

#endif