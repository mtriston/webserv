/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_gc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtriston <mtriston@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/14 23:32:14 by mtriston          #+#    #+#             */
/*   Updated: 2020/10/22 23:31:41 by mtriston         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../libft.h"

void	*malloc_gc(size_t size)
{
	void *ptr;

	ptr = malloc(size);
	memory_manager(ptr, ADD);
	return (ptr);
}

void	*calloc_gc(size_t nmemb, size_t size)
{
	void *ptr;

	ptr = ft_calloc(nmemb, size);
	return (ptr);
}
