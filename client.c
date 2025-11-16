/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rydelepi <rydelepi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 16:07:11 by rydelepi          #+#    #+#             */
/*   Updated: 2025/11/16 16:07:11 by rydelepi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include "Libft/libft.h"
#include "Printf/libftprintf.h"

static int	g_received = 0;


void	handle_sigusr1(int sig)
{
	g_received = 1;
	(void)sig;
}

void	send_byte(int server_pid, unsigned char byte)
{
	int	bit;

	bit = 7;
	while (bit >= 0)
	{
		if ((byte >> bit) & 1)
			kill(server_pid, SIGUSR1);
		else
			kill(server_pid, SIGUSR2);
		usleep(500);
		bit--;
	}
}

int	main(int argc, char **argv)
{
	int				server_pid;
	int				i;
	unsigned char	byte;
	struct sigaction	sa;

	if (argc != 3)
	{
		ft_printf("Usage: %s <server_pid> <message>\n", argv[0]);
		return (1);
	}
	server_pid = ft_atoi(argv[1]);
	
	if (server_pid <= 0)
	{
		ft_printf("Error: Invalid server PID\n");
		return (1);
	}

	ft_bzero(&sa, sizeof(sa));
	sa.sa_handler = handle_sigusr1;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGUSR1, &sa, NULL);

	i = 0;
	while (argv[2][i])
	{
		byte = (unsigned char)argv[2][i];
		send_byte(server_pid, byte);
		i++;
	}
	send_byte(server_pid, '\0');
        if (g_received)
            ft_printf("Message received by server!\n");
	return (0);
}
