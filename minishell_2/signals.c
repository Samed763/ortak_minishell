#include "minishell.h"

int g_signal_received = 0;

void signal_handler_interactive(int sig)
{
    if (sig == SIGINT)
    {
        g_signal_received = SIGINT;
        printf("\n");
        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
    }
}

void signal_handler_heredoc(int sig)
{
    if (sig == SIGINT)
    {
        g_signal_received = SIGINT;
        printf("\n");
        exit(130);
    }
}

void signal_handler_execution(int sig)
{
    if (sig == SIGINT)
    {
        g_signal_received = SIGINT;
        printf("\n");
    }
    else if (sig == SIGQUIT)
    {
        g_signal_received = SIGQUIT;
        printf("Quit: 3\n");
    }
}

void setup_signals(void)
{
    struct sigaction sa_int;
    struct sigaction sa_quit;

    sa_int.sa_handler = signal_handler_interactive;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa_int, NULL);

    sa_quit.sa_handler = SIG_IGN;
    sigemptyset(&sa_quit.sa_mask);
    sa_quit.sa_flags = 0;
    sigaction(SIGQUIT, &sa_quit, NULL);
}

void setup_signals_for_execution(void)
{
    struct sigaction sa_int;
    struct sigaction sa_quit;

    sa_int.sa_handler = signal_handler_execution;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);

    sa_quit.sa_handler = signal_handler_execution;
    sigemptyset(&sa_quit.sa_mask);
    sa_quit.sa_flags = 0;
    sigaction(SIGQUIT, &sa_quit, NULL);
}

void setup_signals_for_heredoc(void)
{
    struct sigaction sa_int;
    struct sigaction sa_quit;

    sa_int.sa_handler = signal_handler_heredoc;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);

    sa_quit.sa_handler = SIG_IGN;
    sigemptyset(&sa_quit.sa_mask);
    sa_quit.sa_flags = 0;
    sigaction(SIGQUIT, &sa_quit, NULL);
}

void restore_signals(void)
{
    struct sigaction sa_default;

    sa_default.sa_handler = SIG_DFL;
    sigemptyset(&sa_default.sa_mask);
    sa_default.sa_flags = 0;

    sigaction(SIGINT, &sa_default, NULL);
    sigaction(SIGQUIT, &sa_default, NULL);
}
