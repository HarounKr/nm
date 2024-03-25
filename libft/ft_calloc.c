/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkrifa <hkrifa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/26 10:13:41 by hkrifa            #+#    #+#             */
/*   Updated: 2021/04/06 13:00:55 by hkrifa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	char	*tab;
	size_t	i;
	size_t	max;

	max = nmemb * size;
	tab = malloc(max);
	if (!tab)
		return (NULL);
	i = 0;
	while (max--)
	{
		tab[i] = 0;
		i++;
	}
	return ((void *)tab);
}
