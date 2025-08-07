/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 16:20:02 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/07 14:39:03 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	is_token(int c)
{
	return (c == '|' || c == '<' || c == '>');
}

int	is_valid_after_pipe(int c)
{
	return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0'
			&& c <= '9') || c == '_' || c == '-' || c == '.' || c == '/'
		|| c == '\'' || c == '"' || c == '$' || c == ' ' || c == '\t');
}

int	is_valid_filename_char(int c)
{
	return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0'
			&& c <= '9') || c == '_' || c == '-' || c == '.' || c == '/'
		|| c == '~' || c == '\'' || c == '"' || c == '$');
}
int is_valid_to_expand(int c)
{
    // Değişken adı harf veya alt çizgi ile başlayabilir.
    // '?' ise özel durum olan $? içindir.
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'
            || c == '?');
}

