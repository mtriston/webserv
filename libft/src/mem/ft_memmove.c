/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtriston <mtriston@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/02 14:35:22 by mtriston          #+#    #+#             */
/*   Updated: 2020/10/22 23:31:40 by mtriston         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	size_t					i;
	const unsigned char		*s;
	unsigned char			*d;

	if (dest == src)
		return (dest);
	i = 0;
	s = (unsigned char *)src;
	d = (unsigned char *)dest;
	if (d > s)
		while (i++ < n)
			d[n - i] = s[n - i];
	else
		while (i < n)
		{
			d[i] = s[i];
			i++;
		}
	return (dest);
}
