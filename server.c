#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include "42_Libft/libft.h"
#include "42_printf/libftprintf.h"

// Global variables to store received data
static int	g_char = 0;
static int	g_bit_count = 0;
static int	g_client_pid = 0;

/*
** Handle SIGUSR1 signal (bit = 1)
** Shifts the character left and adds 1
** When 8 bits are received, processes the complete character
*/
void	handle_sigusr1(int sig, siginfo_t *info, void *context)
{
	// Capture the client PID from the signal info (si_pid)
	// This is set automatically by the kernel
	if (g_bit_count == 0)
		g_client_pid = info->si_pid;
	
	// Bitwise shift left and OR with 1 to add the bit
	g_char = (g_char << 1) | 1;
	g_bit_count++;
	
	// When we have a complete byte (8 bits)
	if (g_bit_count == 8)
	{
		// Character 0 (null terminator) marks end of message
		if (g_char == 0)
		{
			// Print newline to complete the message display
			write(1, "\n", 1);
			// BONUS: Send confirmation signal to client (SIGUSR1 = received)
			if (g_client_pid > 0)
				kill(g_client_pid, SIGUSR1);
			// Reset for next message
			g_client_pid = 0;
		}
		else
		{
			// Write the received character to stdout
			write(1, (char *)&g_char, 1);
		}
		// Reset for next character
		g_char = 0;
		g_bit_count = 0;
	}
	(void)sig;
	(void)context;
}

/*
** Handle SIGUSR2 signal (bit = 0)
** Shifts the character left and adds 0
** When 8 bits are received, processes the complete character
*/
void	handle_sigusr2(int sig, siginfo_t *info, void *context)
{
	// Capture the client PID from the signal info (si_pid)
	// This is set automatically by the kernel
	if (g_bit_count == 0)
		g_client_pid = info->si_pid;
	
	// Bitwise shift left and OR with 0 to add the bit
	g_char = (g_char << 1) | 0;
	g_bit_count++;
	
	// When we have a complete byte (8 bits)
	if (g_bit_count == 8)
	{
		// Character 0 (null terminator) marks end of message
		if (g_char == 0)
		{
			// Print newline to complete the message display
			write(1, "\n", 1);
			// BONUS: Send confirmation signal to client (SIGUSR1 = received)
			if (g_client_pid > 0)
				kill(g_client_pid, SIGUSR1);
			// Reset for next message
			g_client_pid = 0;
		}
		else
		{
			// Write the received character to stdout
			write(1, (char *)&g_char, 1);
		}
		// Reset for next character
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
	
	// Initialize sigaction structure for SIGUSR1
	ft_bzero(&sa_usr1, sizeof(sa_usr1));
	sa_usr1.sa_sigaction = handle_sigusr1;
	sigemptyset(&sa_usr1.sa_mask);
	sa_usr1.sa_flags = SA_SIGINFO;  // Enable siginfo_t parameter
	
	// Initialize sigaction structure for SIGUSR2
	ft_bzero(&sa_usr2, sizeof(sa_usr2));
	sa_usr2.sa_sigaction = handle_sigusr2;
	sigemptyset(&sa_usr2.sa_mask);
	sa_usr2.sa_flags = SA_SIGINFO;  // Enable siginfo_t parameter
	
	// Register signal handlers using sigaction (more reliable than signal)
	sigaction(SIGUSR1, &sa_usr1, NULL);
	sigaction(SIGUSR2, &sa_usr2, NULL);
	
	// Display server PID using ft_printf (client needs this PID)
	ft_printf("Server PID: %d\n", server_pid);
	
	// Wait for signals indefinitely
	while (1)
		pause();
	
	return (0);
}
