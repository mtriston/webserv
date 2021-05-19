/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtriston <mtriston@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/27 19:23:48 by mtriston          #+#    #+#             */
/*   Updated: 2020/10/22 23:31:40 by mtriston         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../libft.h"

static void	write_number(char *dest, unsigned int n, int i)
{
	if (n >= 10)
	{
		write_number(dest, n / 10, i - 1);
		dest[i] = n % 10 + '0';
	}
	else
		dest[i] = n + '0';
}

char		*ft_itoa(int n)
{
	int				i;
	int				num;
	unsigned int	temp_num;
	char			*str;

	num = n;
	i = (n < 0 || n == 0) ? 1 : 0;
	while (num != 0)
	{
		num /= 10;
		i++;
	}
	if (!(str = (char*)malloc_gc((i + 1) * sizeof(char))))
		return (0);
	if (n < 0)
	{
		str[0] = '-';
		temp_num = -n;
	}
	else
		temp_num = n;
	write_number(str, temp_num, i - 1);
	str[i] = '\0';
	return (str);
}
