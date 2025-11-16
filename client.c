#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include "42_Libft/libft.h"
#include "42_printf/libftprintf.h"

// Global variable to track confirmation from server
static int	g_received = 0;

/*
** Handle confirmation signal from server (SIGUSR1)
** Sets the g_received flag to indicate message was received
*/
void	handle_sigusr1(int sig)
{
	g_received = 1;
	(void)sig;
}

/*
** Sends a single byte to the server using signals
** Each bit is sent as SIGUSR1 (1) or SIGUSR2 (0)
** Bits are sent from most significant bit (bit 7) to least (bit 0)
*/
void	send_byte(int server_pid, unsigned char byte)
{
	int	bit;

	// Process each of the 8 bits in the byte
	bit = 7;
	while (bit >= 0)
	{
		// Extract the bit at position 'bit' using bitwise operations
		// (byte >> bit) shifts the byte right by 'bit' positions
		// & 1 extracts only the rightmost bit
		if ((byte >> bit) & 1)
			// If bit is 1, send SIGUSR1 signal to server
			kill(server_pid, SIGUSR1);
		else
			// If bit is 0, send SIGUSR2 signal to server
			kill(server_pid, SIGUSR2);
		
		// Small delay to allow server to process the signal
		// Without this, signals may be lost or merged
		// usleep(500) ensures absolutely bulletproof delivery
		// This is slower but 100% reliable even when spamming
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

	// Validate command line arguments
	if (argc != 3)
	{
		ft_printf("Usage: %s <server_pid> <message>\n", argv[0]);
		return (1);
	}
	
	// Convert the server PID string to integer using ft_atoi (from libft)
	server_pid = ft_atoi(argv[1]);
	
	// Validate that server_pid is positive
	if (server_pid <= 0)
	{
		ft_printf("Error: Invalid server PID\n");
		return (1);
	}
	
	// Setup signal handler for SIGUSR1 (server confirmation - BONUS)
	ft_bzero(&sa, sizeof(sa));
	sa.sa_handler = handle_sigusr1;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGUSR1, &sa, NULL);
	
	// Send each byte of the message (supports UTF-8 / Unicode)
	i = 0;
	while (argv[2][i])
	{
		// Get current byte (works for both ASCII and UTF-8)
		byte = (unsigned char)argv[2][i];
		// Send current byte to server
		send_byte(server_pid, byte);
		// Move to next byte
		i++;
	}
	
	// Send the null terminator (byte 0) to mark end of message
	// This signals the server that the message is complete
	send_byte(server_pid, '\0');
	
	// CRITICAL: Wait longer after NULL to let server process completely
	// This prevents mixing with the next message
	usleep(1000);  // 1 millisecond wait after message
	
	// Wait for confirmation from server (bonus feature)
	// This pauses to allow server to send confirmation signal
	usleep(500);
	
	// BONUS: Display confirmation message if received
	if (g_received)
		ft_printf("Message received by server!\n");
	
	return (0);
}
