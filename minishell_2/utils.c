#include "minishell.h"

void *Malloc(size_t size)
{
    void *ptr;
    if (size == 0)
        return (NULL);
    ptr = malloc(size);
    if (!ptr)
    {
        perror("Malloc BOMM");
        exit(1);
    }   
    return (ptr);
}

int	ft_strlen(const char *str)
{
	int	i;

	i = 0;
	while (str[i] != '\0')
		i++;
	return (i);
}

char	*ft_strdup(const char *s1)
{
	char	*s2;
	size_t	len;

	len = ft_strlen(s1);
	s2 = (char *)malloc(len + 1);
	if ((s2) == NULL)
		return (NULL);
	ft_memcpy(s2, s1, len + 1);
	return (s2);
}
void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	size_t	i;

	i = 0;
	if (!dst && !src)
		return (0);
	while (i < n)
	{
		((char *)dst)[i] = ((char *)src)[i];
		i++;
	}
	return (dst);
}

char ***make_env_dictionary(char **envp)
{
    int i, count = 0;

    while (envp[count])
        count++;

    char ***env_ret = Malloc(sizeof(char **) * (count + 1));
    for (i = 0; i < count; i++)
    {
        /* '=' işaretini bul */
        int pos = 0;
        while (envp[i][pos] && envp[i][pos] != '=')
            pos++;

        /* anahtar ve değer uzunlukları */
        int key_len = pos;
        int value_len = 0;
        if (envp[i][pos] == '=') /* '=' bulunduysa */
            while (envp[i][pos + 1 + value_len])
                value_len++;

        /* 2 string + NULL saklayacak alan */
        env_ret[i] = malloc(sizeof(char *) * 3);

        /* --- key kopyala --- */
        env_ret[i][0] = malloc(key_len + 1);
        for (int k = 0; k < key_len; k++)
            env_ret[i][0][k] = envp[i][k];
        env_ret[i][0][key_len] = '\0';

        /* --- value kopyala (varsa) --- */
        env_ret[i][1] = malloc(value_len + 1);
        for (int v = 0; v < value_len; v++)
            env_ret[i][1][v] = envp[i][pos + 1 + v];
        env_ret[i][1][value_len] = '\0';

        env_ret[i][2] = NULL; /* satır sonu */
    }
    env_ret[count] = NULL; /* tablo sonu */
    return env_ret;
}

char *get_value_by_key(char ***env_dict, const char *key)
{
    int i = 0;
    while (env_dict[i] != NULL)
    {
        if (env_dict[i][0] && env_dict[i][1])
        {
            // strcmp ile karşılaştır
            int j = 0;
            while (key[j] && env_dict[i][0][j] && key[j] == env_dict[i][0][j])
                j++;
            if (key[j] == '\0' && env_dict[i][0][j] == '\0')
                return env_dict[i][1]; // key bulundu, value döndür
        }
        i++;
    }
    return NULL; // bulunamadı
}
void print_env_dictionary(char ***env)
{
    int i = 0;
    while (env[i] != NULL)
    {
        //[i][0] == KEY
        //[i][1] == Value
        if (env[i][0] && env[i][1])                   // key ve value varsa
            printf("%s=%s \n", env[i][0], env[i][1]); // = sonradan ekleniyor
        i += 1;
    }
}
void print_tokens(char **tokens)
{
    
    int i = 0;
    

    if (!tokens)
        return;
    
    printf("\n--- Command Tokens ---\n");
    while (tokens[i])
    {
        printf("Token %d: [%s]\n", i, tokens[i]);
        i++;
    }
    printf("--------------------\n\n");
}

char	*ft_strjoin(char *s1, char *s2)
{
	char	*joined_str;
	int		i;
	int		j;

	i = -1;
	j = 0;
	if (!s1)
	{
		s1 = ft_strdup("");
		if (!s1)
			return (NULL);
		s1[0] = '\0';
	}
	if (!s2)
		return (NULL);
	joined_str = malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
	if (!joined_str)
		return (NULL);
	while (s1[++i] != '\0')
		joined_str[i] = s1[i];
	while (s2[j] != '\0')
		joined_str[i++] = s2[j++];
	joined_str[i] = '\0';
	
	return (joined_str);
}

int	ft_strcmp(const char *s1, const char *s2)
{
	int	i;

	i = 0;
	while (s1[i] != '\0' || s2[i] != '\0')
	{
		if (s1[i] != s2[i])
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		i++;
	}
	return (0);
}

void print_parsed_commands(t_data *data)
{
    int cmd_idx = 0;
    
    if (!data || !data->parsed || !data->parsed->args)
        return;
        
    printf("\n--- Parsed Commands ---\n");
    
    // Iterate through each command segment (separated by pipes)
    while (data->parsed->args[cmd_idx])
    {
        printf("Command %d: ", cmd_idx + 1);
        
        // Print all arguments for this command
        int arg_idx = 0;
        while (data->parsed->args[cmd_idx][arg_idx])
        {
            printf("[%s] ", data->parsed->args[cmd_idx][arg_idx]);
            arg_idx++;
        }
        
        // Print pipe symbol between commands for visualization
        if (data->parsed->args[cmd_idx + 1])
            printf(" | ");
            
        printf("\n");
        cmd_idx++;
    }
    
    // Print redirection information
    if (data->parsed->input_file)
        printf("Input Redirection: < %s\n", data->parsed->input_file);
    if (data->parsed->heredoc)
        printf("Heredoc: << %s\n", data->parsed->heredoc_limiter);
    if (data->parsed->output_file)
        printf("Output Redirection: %s %s\n", 
               data->parsed->append ? ">>" : ">", 
               data->parsed->output_file);
    
    printf("----------------------\n\n");
}