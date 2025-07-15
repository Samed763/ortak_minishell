#include "minishell.h"

static void expand_heredoc_lines(t_data *data,t_command *cmd)
{
    t_heredoc_line *current = cmd->heredoc_lines;
    
    while (current)
    {
        char *expanded = expand_single_line(data,current->content);
        free(current->content);
        current->content = expanded;
        current = current->next;
    }
}

static t_heredoc_line *create_heredoc_node(char *line)
{
    t_heredoc_line *node;
    node = Malloc(sizeof(t_heredoc_line));
    
    node->content = ft_strdup(line);
    if (!node->content)  // ← strdup kontrolü
    {
        free(node);
        return (NULL);
    }
    node->next = NULL;
    return node;
}

void handle_heredoc(t_data *data,t_command *cmd)
{
    t_heredoc_line *head = NULL;
    t_heredoc_line *current = NULL;
    t_heredoc_line *new_node;
    char * line;

    if (!cmd->heredoc_delimiter)  // ✅ NULL kontrolü
        return;
    while (1)
    {
        line = readline("> ");
        if (!line)
            return ;

        if (ft_strcmp(line,cmd->heredoc_delimiter) == 0)
        {
            free(line);
            break; ;
        }
        new_node = create_heredoc_node(line);
        if (!new_node)
        {
            free(line);
            continue;
        }
        if (!head)
        {
            head = new_node;
            current = new_node;
        }
        else{
            current->next = new_node;
            current = new_node;
        }
        free(line);
    }
    cmd->heredoc_lines = head;
    expand_heredoc_lines(data,cmd);
}

// main fonksiyonunu güncelle
int main(int argc, char **argv, char **envp)
{
    char *line;
    t_data data;
    data.env = copy_env(envp);
    data.exit_value = 0;      // ✅ Initialize
    data.cmd = NULL;       
    while (1)
    {
        line = readline("minishell$ ");

        if (!line)
        {
            return 0;
        }

        if (syntax_check(line))
        {
            printf("syntax error\n");
            free(line);
            continue;
        }

        lexer(line,&data);

        expander(&data);

        data.cmd = parser(&data);

        // Memory cleanup
        free_word_array(data.word_array);
        free(data.token);
        free(line);
    }

    return 0;
}