#include "minishell.h"
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
void free_commands(t_command *cmd)
{
    t_command *current = cmd;
    t_command *next;
    
    while (current)
    {
        next = current->next;
        
        // Free args
        if (current->args)
            free_word_array(current->args);
        
        // Free input file
        if (current->input_files)
            free(current->input_files);
        
        // Free output files
        if (current->output_files)
        {
            int i = 0;
            while (i < current->output_count)
            {
                free(current->output_files[i]);
                i++;
            }
            free(current->output_files);
        }
        
        // Free append modes
        if (current->append_modes)
            free(current->append_modes);
        
        // Free heredoc delimiter
        if (current->heredoc_delimiter)
            free(current->heredoc_delimiter);
        
        // Free heredoc lines
        free_heredoc_lines(current->heredoc_lines);
        
        free(current);
        current = next;
    }
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
        execute_commmand(&data);
        // Memory cleanup

    }

    return 0;
}
