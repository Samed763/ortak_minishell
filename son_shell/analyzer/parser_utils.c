/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* parser_utils.c                                     :+:      :+:    :+:   */
/* +:+ +:+         +:+     */
/* By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/* +#+#+#+#+#+   +#+           */
/* Created: 2025/07/30 18:36:57 by sadinc            #+#    #+#             */
/* Updated: 2025/08/08 09:46:00 by sadinc           ###   ########.fr       */
/* */
/* ************************************************************************** */

#include "../minishell.h"

/*
** GÜNCELLENDİ: create_list
** t_command yapısı oluşturulurken tüm işaretçilerin NULL olarak
** başlatıldığından emin oluyoruz. Özellikle yeni eklenen redirs.
*/
t_command	*create_list(void)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->pid = -1;
	cmd->redirs = NULL; // YENİ: Yönlendirme listesini NULL olarak başlat.
	cmd->heredocs = NULL;
	cmd->next = NULL;
	return (cmd);
}

static void	process_quoted_section(char *str, char *new_str, int *i, int *j)
{
	char	quote_char;

	quote_char = str[*i];
	(*i)++;
	while (str[*i] && str[*i] != quote_char)
	{
		new_str[*j] = str[*i];
		(*j)++;
		(*i)++;
	}
	if (str[*i] == quote_char)
		(*i)++;
}

/*
** DEĞİŞMEDİ: Bu fonksiyon hala tırnakları temizlemek için genel
** amaçlı olarak kullanılıyor.
*/
char	*remove_quotes(char *str)
{
	char	*new_str;
	int		i;
	int		j;

	if (!str)
		return (NULL);
	new_str = (char *)malloc(ft_strlen(str) + 1);
	if (!new_str)
		return (NULL);
	i = 0;
	j = 0;
	while (str[i])
	{
		if (str[i] == '\'' || str[i] == '"')
			process_quoted_section(str, new_str, &i, &j);
		else
			new_str[j++] = str[i++];
	}
	new_str[j] = '\0';
	return (new_str);
}


/*
** ARTIK GEREKLİ DEĞİL: add_input_to_command fonksiyonu kaldırıldı.
** remove_quotes_from_word ve remove_quotes_parser_helper fonksiyonları da
** artık bu dosyada kullanılmıyor ve kaldırılabilir. heredoc_utils2.c'de
** benzer bir mantık kuracağız.
*/

char	*remove_quotes_from_word(char *str, int *should_expand)
{
	char	*new_str;
	int		i;
	int		j;

	if (!str)
		return (NULL);
	*should_expand = 1; // Başlangıçta genişletme varsayılan olarak açık
	if (ft_strchr(str, '\'') != NULL)
		*should_expand = 0; // Tek tırnak varsa genişletme olmaz

	new_str = (char *)malloc(ft_strlen(str) + 1);
	if (!new_str)
		return (NULL);
	i = 0;
	j = 0;
	while (str[i])
	{
		if (str[i] == '\'' || str[i] == '"')
			process_quoted_section(str, new_str, &i, &j);
		else
			new_str[j++] = str[i++];
	}
	new_str[j] = '\0';
	return (new_str);
}