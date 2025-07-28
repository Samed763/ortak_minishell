/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 16:15:10 by sadinc            #+#    #+#             */
/*   Updated: 2025/07/28 19:54:04 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	is_identifier_char(int c)
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0'
		&& c <= '9') || (c == '_');
}

char	*put_var(char *line, char *var_value, int key_start, int key_end)
{
	char	*start;
	char	*end;
	char	*temp;
	char	*ret_val;
	char	*safe_value;

	start = ft_substr(line, 0, key_start);
	if (!start)
	{
		free(line);
		return (NULL);
	}
	end = ft_substr(line, key_end, ft_strlen(line));
	if (!end)
	{
		free(start); // Önceki adımlarda ayrılan belleği temizle.
		free(line);
		return (NULL);
	}
	if (!var_value)
		safe_value = ft_strdup("");
	else
		safe_value = ft_strdup(var_value);
	if (!safe_value)
	{
		free(start);
		free(end);
		free(line);
		return (NULL);
	}
	temp = ft_strjoin(safe_value, end);
	free(safe_value); // Artık bunlara ihtiyaç yok, hemen temizle.
	free(end);
	if (!temp)
	{
		free(start);
		free(line);
		return (NULL);
	}
	ret_val = ft_strjoin(start, temp);
	free(start); // Artık bunlara ihtiyaç yok.
	free(temp);
	if (!ret_val)
	{
		free(line);
		return (NULL); // Son birleştirme de başarısız olabilir.
	}
	free(line); // Her şey yolunda gitti, eski line'ı temizle.
	return (ret_val);
}
/*
 * Açıklama: Bu fonksiyon, bir satırdaki değişkenleri genişletir.
 * Bellek ayırma hatası olursa, tüm kaynakları temizleyip NULL döndürür.
 * Değişiklik yapmazsa, orijinal satırın bir kopyasını döndürür.
 * Değişiklik yaparsa, tamamen yeni bir string döndürür.
 * 'put_var' fonksiyonu eski 'result' string'ini zaten free'lediği için
 * bu fonksiyonda tekrar free etmeye gerek kalmaz.
 */
char	*expand_single_line(t_data *data, char *line)
{
	char	*result;
	int		s_quotes;
	int		d_quotes;
	size_t	j;
			char *key;
			char *value;
	int		k;

	// 1. Adım: Orijinal satırı kopyala ve KONTROL ET.
	result = ft_strdup(line);
	if (!result)
		return (NULL);
	s_quotes = 0;
	d_quotes = 0;
	j = 0;
	// 4. Adım: Döngü koşulunu daha güvenli hale getir.
	while (result && result[j])
	{
		if (result[j] == '\'' && d_quotes == 0)
			s_quotes = !s_quotes;
		else if (result[j] == '"' && s_quotes == 0)
			d_quotes = !d_quotes;
		if (result[j] == '$' && s_quotes == 0)
		{
			k = j + 1;
			if (result[k] == '?')
			{
				key = ft_strdup("?");
				k++;
			}
			else
			{
				while (result[k] && is_identifier_char(result[k]))
					k++;
				key = ft_substr(result, j + 1, k - j - 1);
			}
			// 2. Adım: 'key' ayırma işlemini KONTROL ET.
			if (!key)
			{
				free(result); // O ana kadar ayrılan 'result'ı temizle.
				return (NULL);
			}
			value = find_value_by_key(data, key);
			result = put_var(result, value, j, k);
			free(key);
			if (value)
				free(value);
			if (!result)
				return (NULL);
			j = -1;
		}
		j++;
	}
	return (result);
}

/*
 * Açıklama: Bu fonksiyon, komut dizisindeki her bir kelimeyi genişletir.
 * 'expand_single_line' fonksiyonundan NULL dönerse (bellek hatası),
 * programın çökmemesi için orijinal kelimeyi değiştirmeden bırakır.
 * Bu, en güvenli yaklaşımdır.
 */
void	expander(t_data *data)
{
	int		i;
	char	*expanded;

	i = 0;
	if (!data || !data->word_array)
		return ;
	while (data->word_array[i])
	{
		if (i > 0 && data->token[i - 1] == TOKEN_HEREDOC)
		{
			i++;
			continue ;
		}
		expanded = expand_single_line(data, data->word_array[i]);
		if (expanded)
		{
			free(data->word_array[i]);      // Eskiyi sil
			data->word_array[i] = expanded; // Yeniyi ata
		}
		i++;
	}
}
