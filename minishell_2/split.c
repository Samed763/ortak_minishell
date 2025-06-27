/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/02 14:33:17 by sadinc            #+#    #+#             */
/*   Updated: 2025/06/27 17:33:27 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Count the number of substrings that will be created when splitting
 * @param str: String to analyze
 * @param sep: Separator character
 * @return: Number of substrings
 */
static int	str_count(char const *str, char sep)
{
	int	count;
	int	i;

	count = 0;
	i = 0;
	while (str[i])
	{
		if (str[i] != sep && (str[i + 1] == sep || !str[i + 1]))
			count++;
		i++;
	}
	return (count);
}

/**
 * Extract a substring until separator or end of string
 * @param str: Source string
 * @param sep: Separator character
 * @return: Allocated substring
 */
static char	*str_parse(const char *str, char sep)
{
	char	*s;
	int		len;

	len = 0;
	while (str[len] && str[len] != sep)
		len++;
	s = (char *)malloc(sizeof(char) * (len + 1));
	if (!s)
		return (NULL);
	ft_memcpy(s, str, len);
	s[len] = '\0';
	return (s);
}

/**
 * Free all allocated strings in array and the array itself
 * @param arr: Array of strings to free
 * @return: NULL (for return convenience)
 */
static void	*free_all(char **arr)
{
	int	i;

	i = 0;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
	return (NULL);
}

/**
 * Split a string into array of substrings using delimiter
 * @param s: String to split
 * @param c: Delimiter character
 * @return: Array of allocated substrings, NULL terminated
 */
char	**ft_split(char const *s, char c)
{
	char	**arr;
	char	**start;

	if (!s)
		return (NULL);
	arr = (char **)malloc(sizeof(char *) * (str_count(s, c) + 1));
	if (!arr)
		return (NULL);
	start = arr;
	while (*s)
	{
		while (*s && *s == c)
			s++;
		if (*s)
		{
			*arr = str_parse(s, c);
			if (!*arr)
				return (free_all(start));
			arr++;
		}
		while (*s && *s != c)
			s++;
	}
	*arr = NULL;
	return (start);
}