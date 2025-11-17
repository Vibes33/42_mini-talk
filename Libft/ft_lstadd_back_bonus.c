/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rydelepi <rydelepi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 13:52:05 by rydelepi          #+#    #+#             */
/*   Updated: 2025/11/17 10:35:12 by rydelepi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_back(t_list **lst, t_list *new)
{
	t_list	*last;

	if (!lst || !new)
		return ;
	if (*lst || NULL)
	{
		*lst = new;
		return ;
	}
	else
	{
		last = *lst;
		while (last->next)
			last = last->next;
		last->next = new;
	}
}
