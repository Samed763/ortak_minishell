#include "minishell.h"

static void	remove_processed_vars(t_data *data, int processed_count)
{
    int		i;
    int		j;
    char	**new_array;
    int		remaining_count;

    remaining_count = 0;
    while (data->word_array[remaining_count])
        remaining_count++;
    remaining_count -= processed_count;
    if (remaining_count <= 0)
    {
        data->word_array[0] = NULL;
        return ;
    }
    new_array = malloc(sizeof(char *) * (remaining_count + 1));
    if (!new_array)
        return ;
    i = processed_count;
    j = 0;
    while (data->word_array[i])
    {
        new_array[j] = ft_strdup(data->word_array[i]);
        j++;
        i++;
    }
    new_array[j] = NULL;
    free_word_array(data->word_array);
    data->word_array = new_array;
}

int	find_strchr(char *str, int c)
{
    int	i;

    i = 0;
    while (str[i])
    {
        if (str[i] == c)
            return (i + 1);
        i++;
    }
    return (0);
}

char	*take_var_name(char *var_line)
{
    int		i;
    char	*var_name;

    i = 0;
    while (var_line[i] && var_line[i] != '=')
        i++;
    var_name = malloc(i + 1);
    if (!var_name)
        return (NULL);
    i = 0;
    while (var_line[i] && var_line[i] != '=')
    {
        var_name[i] = var_line[i];
        i++;
    }
    var_name[i] = '\0';
    return (var_name);
}

static void	update_export_command(t_data *data, int var_index, int export_index)
{
    char	*temp;

    temp = data->word_array[export_index];
    data->word_array[export_index] = ft_strdup(data->word_array[var_index]);
    free(temp);
}


int	is_valid_var_name(char *str, int equals_pos)
{
    int	i;

    i = 0;
    if (equals_pos <= 0)
        return (0);
    while (i < equals_pos - 1)
    {
        if (!((str[i] >= 'a' && str[i] <= 'z') || 
            (str[i] >= 'A' && str[i] <= 'Z') || 
            (str[i] >= '0' && str[i] <= '9') || 
            str[i] == '_'))
            return (0);
        i++;
    }
    return (1);
}

int	set_var_wa(t_data *data, int var_index)
{
    char	*var_name;
    char	*current_var_name;
    int		i;
    int		found_match;

    var_name = take_var_name(data->word_array[var_index]);
    if (!var_name)
        return (0);
    found_match = 0;
    i = var_index + 1;
    while (data->word_array[i])
    {
        // Herhangi bir komut (export, echo, ls, vb.) + variable name kontrolü
        if (data->word_array[i + 1])
        {
            current_var_name = take_var_name(data->word_array[i + 1]);
            if (current_var_name && !strcmp(var_name, current_var_name))
            {
                update_export_command(data, var_index, i + 1);
                found_match = 1;
                free(current_var_name);
                break ;
            }
            free(current_var_name);
        }
        i++;
    }
    free(var_name);
    return (found_match);
}

void	check_start_var(t_data *data)
{
    int	count;
    int	equals_index;
    int	processed_count;

    count = 0;
    processed_count = 0;
    while (data->word_array[count])
    {
        equals_index = find_strchr(data->word_array[count], '=');
        if (equals_index > 0)
        {
            if (is_valid_var_name(data->word_array[count], equals_index))
            {
                // Sadece eşleşme bulunursa processed_count'u arttır
                if (set_var_wa(data, count))
                    processed_count++;
                else
                    break ; // Eşleşme bulunamadı, işlemi durdur
            }
            else
            {
                printf("Invalid variable name\n");
                break ;
            }
        }
        else
            break ;
        count++;
    }
    // Sadece işlenen variable'ları sil
    if (processed_count > 0)
        remove_processed_vars(data, processed_count);
}

