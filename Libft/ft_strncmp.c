/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rydelepi <rydelepi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:21:59 by rydelepi          #+#    #+#             */
/*   Updated: 2025/10/20 13:36:54 by rydelepi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strncmp(const char *s1, const char *s2, unsigned int n)
{
	unsigned int	i;

	i = 0;
	if (n <= 0)
		return (0);
	while ((s1[i] || s2[i]) && i < n)
	{
		if (s1[i] != s2[i])
			return (s1[i] - s2[i]);
		i++;
	}
	return (0);
}
// int    main(int argc, char **argv)
// {
//    int        result;
//    size_t    n;
// 	(void) argc;
//    n = (size_t)atoi(argv[3]);
//    result = ft_strncmp(argv[1], argv[2], n);

//    printf("resultat : %d",result);
//    return (0);
// }
