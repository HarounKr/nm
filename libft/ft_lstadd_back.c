/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkrifa <hkrifa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/07 18:24:22 by hkrifa            #+#    #+#             */
/*   Updated: 2021/04/08 13:50:10 by hkrifa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_back(t_list **alst, t_list *new)
{
	t_list	*p_courant;

	p_courant = *alst;
	if (*alst)
	{
		while (p_courant -> next != NULL)
			p_courant = p_courant -> next;
		p_courant -> next = new;
		p_courant = new;
	}
	else
		*alst = new;
}
