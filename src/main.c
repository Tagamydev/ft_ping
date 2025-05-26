/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 08:18:02 by samusanc          #+#    #+#             */
/*   Updated: 2025/05/26 07:57:05 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"
#include "libft.h"

int	ft_ping(char *file, t_flags flags, int multiple)
{
	if (file[0] == '-')
		return 0;
	int	i = 1;
	{
		printf("From 192.168.0.182 icmp_seq=%d Destination Host Unreachable\n", i++);
	}
	return 0;
}

int	main(int argc, char **argv)
{
	int		error_counter = 0;
	t_flags	flags = parse_flags(argv);
	
	if (flags.error && flags.error != -1)
	{
		print_help();
		return 1;
	}
	error_counter += ft_ping(argv[0], flags, 1);
	return error_counter;
}
