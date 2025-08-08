/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   itoa.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 16:31:22 by sadinc            #+#    #+#             */
/*   Updated: 2025/07/31 16:32:21 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	get_token_type(char *token)
{
	if (!token)
		return (TOKEN_WORD);
	if (ft_strcmp(token, "|") == 0)
		return (TOKEN_PIPE);
	else if (ft_strcmp(token, "<") == 0)
		return (TOKEN_REDIRECT_IN);
	else if (ft_strcmp(token, ">") == 0)
		return (TOKEN_REDIRECT_OUT);
	else if (ft_strcmp(token, ">>") == 0)
		return (TOKEN_APPEND);
	else if (ft_strcmp(token, "<<") == 0)
		return (TOKEN_HEREDOC);
	else
		return (TOKEN_WORD);
}

static char	*ifzero(char *mem)
{
	mem[0] = '0';
	return (mem);
}

static size_t	intlen(int s)
{
	size_t	len;

	len = 0;
	if (s <= 0)
		len++;
	while (s)
	{
		s = s / 10;
		len++;
	}
	return (len);
}

char	*ft_itoa(int n)
{
	size_t	len;
	char	*mem;
	long	num;

	len = intlen(n);
	mem = (char *)malloc(len + 1);
	if (!mem)
		return (NULL);
	mem[len] = '\0';
	num = n;
	if (n == 0)
		mem = ifzero(mem);
	if (n < 0)
	{
		mem[0] = '-';
		num = -num;
	}
	while (len-- > 0 && num > 0)
	{
		mem[len] = '0' + (num % 10);
		num = num / 10;
	}
	return (mem);
}
