/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkrifa <hkrifa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/23 10:52:07 by hkrifa            #+#    #+#             */
/*   Updated: 2021/04/05 15:11:41 by hkrifa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_isspace(char str)
{
	if (str == '\t' || str == '\n' || str == '\r'
		|| str == '\v' || str == '\f' || str == ' ')
		return (1);
	return (0);
}

int	ft_atoi(const char *str)
{
	size_t	i;
	size_t	nbr;
	size_t	n;

	n = 0;
	i = 0;
	nbr = 0;
	while (ft_isspace(str[i]) == 1)
		i++;
	while (str[i] == '+' || str[i] == '-')
	{
		if (str[i + 1] == '-' || str[i + 1] == '+')
			return (0);
		else if (str[i] == '-')
			n++;
		i++;
	}
	while (str[i] != '\0' && str[i] >= '0' && str[i] <= '9')
	{
		nbr = (nbr * 10) + str[i] - '0';
		i++;
	}
	if (n % 2 == 0)
		return (nbr * 1);
	return (nbr * -1);
}
