/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free_array.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtriston <mtriston@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/08 18:08:17 by mtriston          #+#    #+#             */
/*   Updated: 2020/11/08 18:10:46 by mtriston         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../libft.h"

void	ft_free_array(char **array, void (*del)(void*))
{
	size_t i;

	i = 0;
	while (array[i] != NULL)
	{
		del(array[i]);
		i++;
	}
	del(array[i]);
	del(array);
}
