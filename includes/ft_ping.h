/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 08:54:32 by samusanc          #+#    #+#             */
/*   Updated: 2025/05/26 15:49:12 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING_H
# define FT_PING_H
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

typedef	struct s_header{
	size_t	addr;
	char	type_char;
	char	*name;
}	t_header;

typedef struct s_flags{

/*-c (count)
Stop sending pings after N requests.
→ Just use a counter. Easy logic around loop iteration.*/
	int		c;
	size_t	number;

/*-w (deadline)
Exit after N seconds, regardless of how many pings sent.
→ Track elapsed time with gettimeofday() or clock_gettime().*/
	int		w;
	size_t	deadline;

/*-W (timeout for reply)
Timeout for each reply (how long to wait for a response).
→ Set recvfrom() timeout with setsockopt() using SO_RCVTIMEO.*/
	int W;
	size_t	timeout;

/* You may specify up to 16 "pad" bytes to fill out the packet you send.  This is useful for diagnosing data-dependent problems in a network.  For exam‐
 ple, “-p ff” will cause the sent packet to be filled with all ones.*/
	int	p;
	// how can i verify this???
	char	*pad;


	int q;
/*
-n (numeric output only)
Disable DNS lookup for hosts.
→ Just avoid gethostbyaddr() or similar when printing IP.
*/
	int n;

	//?
	int i;
	int	interval;
	int	total;
	int	error;
} t_flags;

void	print_help();
int		error(char *title, char *message, int i);
void	parse_flag(t_flags *result, char *str);
t_flags	parse_flags(char **argv);
void	*free_header(void *ptr);
char	*convert_addr(int addr, int is_64);

#endif
