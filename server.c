#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include "42_Libft/libft.h"
#include "42_printf/libftprintf.h"

static int	g_char = 0;
static int	g_bit_count = 0;
static int	g_client_pid = 0;

void	handle_sigusr1(int sig, siginfo_t *info, void *context)
{
	if (g_bit_count == 0)
		g_client_pid = info->si_pid;
	
	g_char = (g_char << 1) | 1;
	g_bit_count++;
	
	if (g_bit_count == 8)
	{
		if (g_char == 0)
		{
			write(1, "\n", 1);
			if (g_client_pid > 0)
				kill(g_client_pid, SIGUSR1);
			g_client_pid = 0;
		}
		else
		{
			write(1, (char *)&g_char, 1);
		}
		g_char = 0;
		g_bit_count = 0;
	}
	(void)sig;
	(void)context;
}

void	handle_sigusr2(int sig, siginfo_t *info, void *context)
{
	if (g_bit_count == 0)
		g_client_pid = info->si_pid;
	
	g_char = (g_char << 1) | 0;
	g_bit_count++;
	
	if (g_bit_count == 8)
	{
		if (g_char == 0)
		{
			write(1, "\n", 1);
			if (g_client_pid > 0)
				kill(g_client_pid, SIGUSR1);
			g_client_pid = 0;
		}
		else
		{
			write(1, (char *)&g_char, 1);
		}
		g_char = 0;
		g_bit_count = 0;
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
