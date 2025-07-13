#include "minishell.h"

// ...existing code...

void expand_heredoc_lines(t_command *cmd, t_data *data)
{
    t_heredoc_line *current = cmd->heredoc_lines;
    
    while (current)
    {
        char *expanded = expand_single_line(current->content, data);
        free(current->content);
        current->content = expanded;
        current = current->next;
    }
}

void debug_heredoc(t_command *cmd)
{
    t_heredoc_line *current;
    int line_count = 0;
    
    printf("\n=== HEREDOC DEBUG ===\n");
    
    if (!cmd)
    {
        printf("Command is NULL\n");
        printf("===================\n\n");
        return;
    }
    
    if (cmd->heredoc_delimiter)
        printf("Delimiter: '%s'\n", cmd->heredoc_delimiter);
    else
        printf("No delimiter found\n");
    
    if (cmd->heredoc_lines)
    {
        printf("Heredoc content:\n");
        current = cmd->heredoc_lines;
        
        while (current)
        {
            if (current->content)  // ← NULL kontrolü ekle
                printf("Line %d: '%s'\n", line_count++, current->content);
            else
                printf("Line %d: (NULL)\n", line_count++);
            current = current->next;
        }
        
        printf("Total lines: %d\n", line_count);
    }
    else
    {
        printf("No heredoc lines found\n");
    }
    
    printf("===================\n\n");
}

void debug_all_heredocs(t_command *commands)
{
    t_command *current = commands;
    int cmd_num = 0;
    
    printf("\n=== ALL HEREDOCS DEBUG ===\n");
    
    if (!current)  // ← NULL kontrolü ekle
    {
        printf("No commands found\n");
        printf("========================\n\n");
        return;
    }
    
    while (current)
    {
        printf("Command %d:\n", cmd_num++);
        debug_heredoc(current);
        current = current->next;
    }
    
    printf("========================\n\n");
}
static t_heredoc_line	*create_heredoc_node(char *content)
{
    t_heredoc_line *h_line;

    h_line = Malloc(sizeof(t_heredoc_line));
    h_line->content = ft_strdup(content);  // ← Content'i direkt ata
    if (!h_line->content)  // ← strdup kontrolü
    {
        free(h_line);
        return (NULL);
    }
    h_line->next = NULL;
    return (h_line);
}

// Memory cleanup fonksiyonu
void free_heredoc_lines(t_heredoc_line *head)
{
    t_heredoc_line *current = head;
    t_heredoc_line *next;
    
    while (current)
    {
        next = current->next;
        free(current->content);
        free(current);
        current = next;
    }
}


void handle_heredoc(t_command *cmd, char *delimiter,t_data *data)
{
    t_heredoc_line *head = NULL;
    t_heredoc_line *current = NULL;
    t_heredoc_line *new_node;
    char *line;
    
    printf("Enter heredoc content (end with '%s'):\n", delimiter);
    
    while (1)
    {
        line = readline("> ");
        if (!line)  // Ctrl+D
        {
            printf("warning: here-document delimited by end-of-file\n");
            break;
        }
        
        // Delimiter kontrolü - EŞİT olduğunda 0 döner
        if (ft_strcmp(line, delimiter) == 0)
        {
            free(line);
            break;  // Heredoc bitir
        }
        
        // Yeni node oluştur
        new_node = create_heredoc_node(line);
        if (!new_node)
        {
            free(line);
            continue;
        }
        
        // Linked list'e ekle
        if (!head)  // İlk node
        {
            head = new_node;
            current = new_node;
        }
        else  // Sonraki node'lar
        {
            current->next = new_node;
            current = new_node;
        }
        
        free(line);
    }
    
    // cmd'a ata
    cmd->heredoc_lines = head;
    expand_heredoc_lines(cmd,data);

    
}