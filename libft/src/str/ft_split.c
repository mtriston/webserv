/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtriston <mtriston@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/04 13:48:24 by mtriston          #+#    #+#             */
/*   Updated: 2020/10/22 23:31:40 by mtriston         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../libft.h"

static int	count_words(char const *s, char *c)
{
	int count;
	int i;

	count = 0;
	i = 0;
	while (s && s[i])
	{
		if (!ft_strchr(c, s[i]))
		{
			count++;
			while (!ft_strchr(c, s[i]) && s[i] && s[i + 1])
				i++;
		}
		i++;
	}
	return (count);
}

static int	count_len(char const *str, char *c)
{
	int i;

	i = 0;
	while (str && str[i] && !ft_strchr(c, str[i]))
		i++;
	return (i);
}

static void	*ft_free_gc(char **tab)
{
	int i;

	i = 0;
	while (tab[i] != NULL)
		free_gc(tab[i++]);
	free_gc(tab);
	return (NULL);
}

char		**ft_split(char const *s, char *c)
{
	int		i;
	int		j;
	int		word_count;
	char	*str;
	char	**array;

	str = (char *)s;
	i = 0;
	j = 0;
	word_count = count_words(s, c);
	if (!s || !(array = (char **)malloc_gc((word_count + 1) * sizeof(char *))))
		return (NULL);
	while (j < word_count)
	{
		i = count_len(str, c);
		if (i > 0)
		{
			if (!(array[j++] = ft_substr(str, 0, i)))
				return (ft_free_gc(array));
		}
		i++;
		str += i;
	}
	array[j] = NULL;
	return (array);
}
