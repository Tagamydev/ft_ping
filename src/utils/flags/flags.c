/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   flags.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 02:00:49 by samusanc          #+#    #+#             */
/*   Updated: 2025/12/11 20:00:00 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"
#include <stdlib.h>  // For atof() and atoll()

// Check if string contains only digits
int check_number(char *str)
{
    if (!str || !str[0])
        return (0);
    for (int i = 0; str[i]; i++)
        if (str[i] < '0' || str[i] > '9')
            return (0);
    return (1);
}

// Check if string is a valid float (for interval and timeout)
int check_float(char *str)
{
    if (!str || !str[0])
        return (0);
    
    int has_dot = 0;
    int has_digit = 0;
    
    for (int i = 0; str[i]; i++)
    {
        if (str[i] == '.')
        {
            if (has_dot)  // Second dot found
                return (0);
            has_dot = 1;
        }
        else if (str[i] >= '0' && str[i] <= '9')
        {
            has_digit = 1;
        }
        else
        {
            return (0);  // Invalid character
        }
    }
    
    return has_digit;  // Must have at least one digit
}

// Check if string contains only hexadecimal digits
int check_pad(char *str)
{
    if (!str || !str[0])
        return (0);
    // Pad must be even length (each byte = 2 hex chars) and max 32 chars (16 bytes)
    size_t len = strlen(str);
    if (len % 2 != 0 || len > 32)
        return (0);
    for (int i = 0; str[i]; i++)
        if (!isxdigit((unsigned char)str[i]))
            return (0);
    return (1);
}

// Check if flag requires a value
int flag_needs_value(char flag)
{
    return (flag == 'c' || flag == 'w' || flag == 'W' || 
            flag == 'p' || flag == 'i');
}

// Parse and validate flag value based on flag type
int parse_flag_value(t_flags *flags, char flag, char *value)
{
    if (!value)
    {
        fprintf(stderr, "ft_ping: option requires an argument -- '%c'\n", flag);
        return (-1);
    }

    switch (flag)
    {
        case 'c':
            if (!check_number(value))
            {
                fprintf(stderr, "ft_ping: invalid count: %s\n", value);
                return (-1);
            }
            flags->number = atoll(value);
            if (flags->number == 0)
            {
                fprintf(stderr, "ft_ping: invalid count: %s\n", value);
                return (-1);
            }
            break;

        case 'w':
            if (!check_number(value))
            {
                fprintf(stderr, "ft_ping: invalid deadline: %s\n", value);
                return (-1);
            }
            flags->deadline = atoll(value);
            if (flags->deadline == 0)
            {
                fprintf(stderr, "ft_ping: invalid deadline: %s\n", value);
                return (-1);
            }
            break;

        case 'W':
            if (!check_number(value))
            {
                fprintf(stderr, "ft_ping: invalid timeout: %s\n", value);
                return (-1);
            }
            flags->timeout = atof(value);
            if (flags->timeout <= 0.0)
            {
                fprintf(stderr, "ft_ping: invalid timeout: %s\n", value);
                return (-1);
            }
            break;

        case 'p':
            if (!check_pad(value))
            {
                fprintf(stderr, "ft_ping: invalid pad pattern: %s\n", value);
                return (-1);
            }
            flags->pad = strdup(value);
            if (!flags->pad)
                return (-1);
            break;

        case 'i':
            // -i accepts float values (in seconds)
            if (!check_float(value))
            {
                fprintf(stderr, "ft_ping: invalid interval2: %s\n", value);
                return (-1);
            }
            flags->interval = atof(value);
            if (flags->interval <= 0.0)
            {
                fprintf(stderr, "ft_ping: invalid interval3: %s\n", value);
                return (-1);
            }
            break;

        default:
            return (-1);
    }
    return (0);
}

int process_flag_char(t_flags *flags, char flag)
{
    switch (flag)
    {
        case 'q':
            flags->q = 1;
            break;
        case 'v':
            flags->v++;
            break;
        case 'h':
            flags->help = 1;
            flags->error = -1;
            break;
        default:
            fprintf(stderr, "ft_ping: invalid option -- '%c'\n", flag);
            return (-1);
    }
    return (0);
}

void *parse_flags(char **argv, t_ping *result)
{
    int i = 0;
    int waiting_for_value = 0;
    char pending_flag = 0;
    result->flags.interval = 1.0;
    result->flags.timeout = 10.0;
    
    while (argv[i] && !result->flags.error)
    {
        char *arg = argv[i];
        
        if (waiting_for_value)
        {
            if (parse_flag_value(&result->flags, pending_flag, arg) < 0)
            {
                result->flags.error = 1;
                return (result);
            }
            
            switch (pending_flag)
            {
                case 'c': result->flags.c = 1; break;
                case 'w': result->flags.w = 1; break;
                case 'W': result->flags.W = 1; break;
                case 'p': result->flags.p = 1; break;
                case 'i': result->flags.i = 1; break;
            }
            
            waiting_for_value = 0;
            pending_flag = 0;
            i++;
            continue;
        }
        
        if (arg[0] != '-' || strlen(arg) < 2)
        {
            t_ip *ip = new_ip(arg);
            if (!ip)
            {
                fprintf(stderr, "ft_ping: error parsing address: %s\n", arg);
                return (NULL);
            }
            list_push_b(&result->ips, node(ip, free_ip));
            i++;
            continue;
        }

        if (strcmp(arg, "--") == 0)
        {
            i++;
            while (argv[i])
            {
                t_ip *ip = new_ip(argv[i]);
                if (!ip)
                {
                    fprintf(stderr, "ft_ping: error parsing address: %s\n", argv[i]);
                    return (NULL);
                }
                list_push_b(&result->ips, node(ip, free_ip));
                i++;
            }
            break;
        }

        for (int j = 1; arg[j]; j++)
        {
            char flag = arg[j];
            
            if (flag_needs_value(flag))
            {
                char *value = NULL;
                
                if (arg[j + 1])
                    value = &arg[j + 1];
                else if (argv[i + 1])
                {
                    value = argv[i + 1];
                    i++;
                }
                
                if (parse_flag_value(&result->flags, flag, value) < 0)
                {
                    result->flags.error = 1;
                    return (result);
                }
                
                switch (flag)
                {
                    case 'c': result->flags.c = 1; break;
                    case 'w': result->flags.w = 1; break;
                    case 'W': result->flags.W = 1; break;
                    case 'p': result->flags.p = 1; break;
                    case 'i': result->flags.i = 1; break;
                }
                
                break;
            }
            else
            {
                if (process_flag_char(&result->flags, flag) < 0)
                {
                    result->flags.error = 1;
                    return (result);
                }
            }
        }
        
        i++;
    }
    
    result->flags.total = i;
    return (result);
}