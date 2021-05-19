/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtriston <mtriston@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/02 23:08:21 by mtriston          #+#    #+#             */
/*   Updated: 2020/10/22 23:31:40 by mtriston         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../libft.h"

static int		check_set(char const *s1, char const *set, size_t len)
{
	size_t i;

	i = 0;
	while (i < len)
	{
		if (set[i] == *s1)
			return (1);
		else
			i++;
	}
	return (0);
}

char			*ft_strtrim(char const *s1, char const *set)
{
	char	*str;
	int		len;
	int		set_len;

	if (!s1)
		return (NULL);
	if (!set)
		return (ft_strdup(s1));
	set_len = ft_strlen(set);
	while (*s1 && check_set(s1, set, set_len))
		s1++;
	len = ft_strlen(s1);
	while (len && check_set(s1 + len - 1, set, set_len))
		len--;
	if (!(str = (char *)malloc_gc((len + 1) * sizeof(char))))
		return (NULL);
	ft_strlcpy(str, s1, len + 1);
	return (str);
}
