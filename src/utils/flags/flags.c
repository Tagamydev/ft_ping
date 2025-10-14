/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   flags.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 02:00:49 by samusanc          #+#    #+#             */
/*   Updated: 2025/05/26 17:29:26 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

int	check_number(char *str)
{
	return (0);

}

int	check_pad(char *str)
{
	return (0);
}

int		parse_flag(int *value_flag, char *last_flag, char *flag)
{
	if (flag[0] != '-')
	{
		if (*value_flag)
		{
			*value_flag = 0;
			return (2);
		}
		return (0);
	}
	return (1);
}

void	parse_flags(char **argv, t_ping *result)
{
	int		value_flag = 0;
	char	last_flag = 0;

	for (int i = 0; argv[i] && !result->flags.error; i++) {
		int	arg_type = parse_flag(&value_flag, &last_flag, argv[i]);
		if (!arg_type)
			list_push_b(&result->ips, node(new_ip(argv[i]), free_ip));
		if (arg_type == -1)
			return ;
		if (arg_type == 0)
			continue ;
		switch (last_flag)
		{
			case 'i':
				break ;

		}
	}
}