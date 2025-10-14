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
	for (int i = 0; str[i]; i++)
		if (str[i] < '0' || str[i] > '9')
			return (0);
	return (1);
}

int	check_pad(char *str)
{
	for (int i = 0; str[i]; i++)
		if (!isxdigit((unsigned char)str[i]))
			return (0);
	return (1);
}

int	parse_flag(int *value_flag, char *last_flag, char *flag)
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
	if (strlen(flag) < 2)
		return (-1);
	*last_flag = flag[1];
	*value_flag = 1;
	return (1);
}

void	*parse_flags(char **argv, t_ping *result)
{
	int		value_flag = 0;
	char	last_flag = 0;

	for (int i = 0; argv[i] && !result->flags.error; i++)
	{
		int arg_type = parse_flag(&value_flag, &last_flag, argv[i]);

		if (arg_type == -1)
			return result;

		if (arg_type == 0)
		{
			t_ip *ip = new_ip(argv[i]);
			if (!ip)
			{
				printf("ft_ping: error doing ip\n");
				return (NULL);
			}
			list_push_b(&result->ips, node(ip, free_ip));
			continue;
		}

		switch (last_flag)
		{
			case 'c': result->flags.c = 1; break;
			case 'w': result->flags.w = 1; break;
			case 'W': result->flags.W = 1; break;
			case 'p': result->flags.p = 1; break;
			case 'q': result->flags.q = 1; break;
			case 'i': result->flags.i = 1; break;
			case 'v': result->flags.v = 1; break;
			case 'h':
				result->flags.help = 1;
				result->flags.error = -1;
				break;
			default:
				fprintf(stderr, "Unknown flag: -%c\n", last_flag);
				result->flags.error = 1;
				break;
		}
	}
	return (result);
}