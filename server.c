/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rydelepi <rydelepi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 16:07:19 by rydelepi          #+#    #+#             */
/*   Updated: 2025/11/16 16:07:19 by rydelepi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include "Libft/libft.h"
#include "Printf/libftprintf.h"
#include "mini.h"

static t_server_state	g_state = {0, 0, 0};

void	handle_sigusr1(int sig, siginfo_t *info, void *context)
{
	if (g_state.g_bit_count == 0)
		g_state.g_client_pid = info->si_pid;
	g_state.g_char = (g_state.g_char << 1) | 1;
	g_state.g_bit_count++;
	if (g_state.g_bit_count == 8)
	{
		if (g_state.g_char == 0)
		{
			if (g_state.g_client_pid > 0)
				kill(g_state.g_client_pid, SIGUSR1);
			g_state.g_client_pid = 0;
		}
		else
			write(1, (char *)&g_state.g_char, 1);
		g_state.g_char = 0;
		g_state.g_bit_count = 0;
	}
	(void)sig;
	(void)context;
}

void	handle_sigusr2(int sig, siginfo_t *info, void *context)
{
	if (g_state.g_bit_count == 0)
		g_state.g_client_pid = info->si_pid;
	g_state.g_char = (g_state.g_char << 1) | 0;
	g_state.g_bit_count++;
	if (g_state.g_bit_count == 8)
	{
		if (g_state.g_char == 0)
		{
			if (g_state.g_client_pid > 0)
				kill(g_state.g_client_pid, SIGUSR1);
			g_state.g_client_pid = 0;
		}
		else
			write(1, (char *)&g_state.g_char, 1);
		g_state.g_char = 0;
		g_state.g_bit_count = 0;
	}
	(void)sig;
	(void)context;
}

int	main(void)
{
	struct sigaction	sa_usr1;
	struct sigaction	sa_usr2;
	int					server_pid;

	server_pid = getpid();
	ft_bzero(&sa_usr1, sizeof(sa_usr1));
	sa_usr1.sa_sigaction = handle_sigusr1;
	sigemptyset(&sa_usr1.sa_mask);
	sa_usr1.sa_flags = SA_SIGINFO;
	ft_bzero(&sa_usr2, sizeof(sa_usr2));
	sa_usr2.sa_sigaction = handle_sigusr2;
	sigemptyset(&sa_usr2.sa_mask);
	sa_usr2.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &sa_usr1, NULL);
	sigaction(SIGUSR2, &sa_usr2, NULL);
	ft_printf("Server PID: %d\n", server_pid);
	while (1)
		pause();
	return (0);
}
