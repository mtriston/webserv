/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_manager.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtriston <mtriston@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/14 23:32:18 by mtriston          #+#    #+#             */
/*   Updated: 2020/10/22 23:31:41 by mtriston         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../libft.h"

static void		gc_lstclear(t_list **lst)
{
	t_list *ptr;
	t_list *temp;

	if (lst && *lst)
	{
		ptr = *lst;
		while (ptr)
		{
			temp = ptr;
			ptr = ptr->next;
			free(temp->content);
			free(temp);
		}
		*lst = NULL;
	}
}

static t_list	*gc_lstnew(void *content)
{
	t_list *elem;

	if (!(elem = malloc(sizeof(t_list))))
		return (NULL);
	elem->content = content;
	elem->next = NULL;
	return (elem);
}

static void		gc_lstdelone(t_list *lst, void (*del)(void*))
{
	if (lst != NULL)
	{
		del(lst->content);
		free(lst);
	}
}

static void		gc_lstremove(t_list **root, void *data)
{
	t_list	*node;
	t_list	*temp;
	int		i;

	while (*root && (data == (*root)->content))
	{
		temp = (*root)->next;
		gc_lstdelone(*root, free);
		(*root) = temp;
	}
	node = *root;
	temp = *root;
	i = 0;
	while (node != NULL)
	{
		if (data == node->content)
		{
			temp->next = node->next;
			gc_lstdelone(node, free);
			node = temp;
			i = 0;
		}
		temp = i++ > 0 ? temp->next : temp;
		node = node != NULL ? node->next : node;
	}
}

void			memory_manager(void *ptr, int action)
{
	static t_list *collector = NULL;

	if (ptr == NULL)
		gc_lstclear(&collector);
	else if (action == ADD)
		ft_lstadd_front(&collector, gc_lstnew(ptr));
	else if (action == REMOVE)
		gc_lstremove(&collector, ptr);
}
