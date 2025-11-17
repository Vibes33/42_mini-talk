/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rydelepi <rydelepi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 10:32:03 by rydelepi          #+#    #+#             */
/*   Updated: 2025/11/17 17:59:37 by rydelepi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINI_H
# define MINI_H

# define _POSIX_C_SOURCE 200109L
# define _XOPEN_SOURCE 500

#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include "Libft/libft.h"
#include "Printf/libftprintf.h"

typedef struct s_server_state
{

	int		g_bit_count;
	int		g_client_pid;
	char	g_char;
	char	*buffer;
	char	*new;
}				t_server_state;

void	buffer_join(t_server_state *g_state);

#endif