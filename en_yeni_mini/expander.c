#include "minishell.h"

char * put_var(char * line,char *var_value,int key_start,int key_end)
{
    char *start = ft_substr(line,0,key_start);
    char *end = ft_substr(line,key_end,ft_strlen(line));
    printf("|start:%s,  ,end:%s|\n",start,end);
    char * ret_val = ft_strjoin(start,ft_strjoin(var_value,end));
    printf("ret_val:%s|\n",ret_val);
    return ret_val;
}



void set_var(t_data *data)
{

    //burada ' tırnak kontorlu yapılcak
    int s_quotes = 0;
    for (size_t i = 0; data->word_array[i]; i++)
    {
        for (size_t j = 0; data->word_array[i][j]; j++)
        {
            if(data->word_array[i][j] == '\'')
                s_quotes = !s_quotes;
            if (data->word_array[i][j] == '$' && s_quotes == 0)
            {
                int k = j  + 1;
                while (data->word_array[i][k] != ' ' && data->word_array[i][k] != '/' && data->word_array[i][k] != '$' && data->word_array[i][k] != '\0')
                    k++;                
                char *key = ft_substr(data->word_array[i],j + 1,k-j-1);
                printf("key :%s|\n",key);
                char * value = find_value_by_key(data,key);
                printf("Value :%s|\n\n",value);
                data->word_array[i] = put_var(data->word_array[i],value,j,k);
            }
        }
    }
}

void expander(t_data * data)
{
    //tırnaklerı kaldır belki geçici belki tamamen
    int i = 0;
    while (data->word_array[i])
    {
        data->word_array[i] = remove_d_quotes(data->word_array[i]);
        i++;
    }
    
    set_var(data);
}