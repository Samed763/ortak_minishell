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
    
    if (!cmd->heredoc_delimiter)
        return;
    while (1)
    {
        line = readline("> ");
        if (!line)
        {
            break;
        }
        if (ft_strcmp(line,cmd->heredoc_delimiter) == 0)
        {
            free(line);
            // Delimiter bulundu ama hiç satır yoksa boş satır ekle
            break;
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
    if (head)
        expand_heredoc_lines(data,cmd);
}
