/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilis.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rydelepi <rydelepi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 17:55:43 by rydelepi          #+#    #+#             */
/*   Updated: 2025/11/17 18:10:08 by rydelepi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mini.h"

void	buffer_join(t_server_state *g_state)
{
	if (!g_state->buffer)
		g_state->buffer = ft_strdup("");
	g_state->new = ft_strjoin(g_state->buffer, &g_state->g_char);
	if (g_state->buffer)
		free(g_state->buffer);
	if (!g_state->new)
		return ;
	g_state->buffer = g_state->new;
}
