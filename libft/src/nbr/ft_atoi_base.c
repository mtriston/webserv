/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi_base.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtriston <mtriston@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/01 21:38:15 by mtriston          #+#    #+#             */
/*   Updated: 2020/10/22 23:31:40 by mtriston         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../libft.h"

int		search_nbr(char *base, char nbr)
{
	int i;

	i = 0;
	while (base[i] != '\0')
	{
		if (base[i] == nbr)
			return (i);
		i++;
	}
	return (-1);
}

int		ft_atoi_base(const char *nptr, char *base)
{
	long	num;
	int		sign;
	int		base_len;

	num = 0;
	sign = 1;
	base_len = ft_strlen(base);
	while (*nptr && ft_isspace(*nptr))
		nptr++;
	if (*nptr == '+' || *nptr == '-')
	{
		if (*nptr == '-')
			sign = -1;
		nptr++;
	}
	while (ft_strchr(base, *nptr) && *nptr)
		num = num * base_len + search_nbr(base, *nptr++);
	return ((int)(num * sign));
}
