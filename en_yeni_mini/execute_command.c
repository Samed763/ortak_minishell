#include "minishell.h"

char	*find_value_by_key(t_data *data, char *key)
{
    int		i;
    char	*search;

    i = 0;
    search = ft_strjoin(key, "=");
    if (!search)
        return (NULL);
    while (data->env[i])
    {
        if (!strncmp(data->env[i], search, ft_strlen(search)))
        {
            free(search);
            return (ft_strdup(data->env[i] + ft_strlen(search)));
        }
        i++;
    }
    free(search);
    return (NULL);
}

int	is_accessable(char **splited_path, char *command)
{
    int		i;
    char	*temp_path;
    char	*full_path;

    i = 0;
    while (splited_path[i])
    {
        temp_path = ft_strjoin(splited_path[i], "/");
        full_path = ft_strjoin(temp_path, command);
        free(temp_path);
        if (access(full_path, X_OK) == 0)
        {
            free(full_path);
            return (i);
        }
        free(full_path);
        i++;
    }
    return (-1);
}

void	execute(char *full_path, t_data *data)
{
    if (execve(full_path, data->cmd->args, data->env) == -1)
    {
        perror("execve");
        exit(1);
    }
}

void	pipe_execute(t_data *data, char **splitted_path)
{
    char	*full_path;
    char	*temp_path;
    int		path_i;
    pid_t	pid;

    if (!data->cmd || !data->cmd->args || !data->cmd->args[0])
        return ;
    path_i = is_accessable(splitted_path, data->cmd->args[0]);
    if (path_i == -1)
    {
        printf("\nminishell: %s: command not found\n", data->cmd->args[0]);
        return ;
    }
    temp_path = ft_strjoin(splitted_path[path_i], "/");
    full_path = ft_strjoin(temp_path, data->cmd->args[0]);
    free(temp_path);
    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        free(full_path);
        return ;
    }
    if (pid == 0)
        execute(full_path, data);
    else
    {
        free(full_path);
        wait(NULL);
    }
}

void	single_execute(t_data *data, char **splitted_path)
{
    char	*full_path;
    char	*temp_path;
    int		path_i;
    pid_t	pid;

    if (!data->cmd || !data->cmd->args || !data->cmd->args[0])
        return ;
    path_i = is_accessable(splitted_path, data->cmd->args[0]);
    if (path_i == -1)
    {
        printf("\nminishell: %s: command not found\n", data->cmd->args[0]);
        return ;
    }
    temp_path = ft_strjoin(splitted_path[path_i], "/");
    full_path = ft_strjoin(temp_path, data->cmd->args[0]);
    free(temp_path);
    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        free(full_path);
        return ;
    }
    if (pid == 0)
        execute(full_path, data);
    else
    {
        free(full_path);
        wait(NULL);
    }
}

void	execute_command(t_data *data)
{
    char	**splitted_path;

    splitted_path = ft_split(find_value_by_key(data, "PATH"), ':');
    if (!splitted_path)
        return ;
    if (!data->cmd->next)
        single_execute(data, splitted_path);
    else
        pipe_execute(data, splitted_path);
    free_word_array(splitted_path);
}