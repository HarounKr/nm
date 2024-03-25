/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkrifa <hkrifa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/07 15:25:08 by hkrifa            #+#    #+#             */
/*   Updated: 2021/04/08 13:50:33 by hkrifa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void*))
{
	t_list	*p_courant;
	t_list	*tmp;

	if (lst)
	{
		p_courant = *lst;
		while (p_courant)
		{
			tmp = p_courant -> next;
			ft_lstdelone(p_courant, del);
			p_courant = tmp;
		}
		*lst = NULL;
	}
}
