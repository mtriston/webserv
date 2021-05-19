/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa_base.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtriston <mtriston@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/06 13:28:35 by mtriston          #+#    #+#             */
/*   Updated: 2020/10/22 23:31:40 by mtriston         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../libft.h"

static size_t	ft_nbrlen(ptrdiff_t n, int base)
{
	size_t i;

	i = (n == 0 || n < 0) ? 1 : 0;
	while (n != 0)
	{
		n /= base;
		i++;
	}
	return (i);
}

static int		check_base(char *base)
{
	char c;

	c = *base;
	while (*base && ft_isprint(*base) && !(ft_strchr(++base, c)))
		c = *base;
	return (*base == '\0' ? 1 : 0);
}

static void		write_number(char *dest, size_t n, char *base, size_t base_len)
{
	if (n >= base_len)
	{
		write_number(dest - 1, n / base_len, base, base_len);
		*dest = base[n % base_len];
	}
	else
		*dest = base[n % base_len];
}

char			*ft_itoa_base(ptrdiff_t n, char *base)
{
	char	*str;
	size_t	nbr_len;
	size_t	base_len;
	size_t	temp;

	base_len = ft_strlen(base);
	if (!base || base_len < 2 || !check_base(base))
		return (NULL);
	nbr_len = ft_nbrlen(n, base_len);
	if (!(str = (char*)malloc_gc((nbr_len + 1) * sizeof(char))))
		return (NULL);
	str[nbr_len] = '\0';
	if (n < 0)
		str[0] = '-';
	temp = (n >= 0) ? n : -n;
	write_number(&str[nbr_len - 1], temp, base, base_len);
	return (str);
}
