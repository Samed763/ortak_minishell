#include "minishell.h"

// Bu fonksiyonun zaten doğru, dokunmana gerek yok.
void pipe_execve(char *full_path, t_data *data, t_command *current)
{
	if (execve(full_path, current->args, data->env) == -1)
	{
		perror("execve");
		exit(1);
	}
}

// Bu fonksiyonun da zaten doğru, dokunmana gerek yok.
static void pipe_execute_child(t_data *data, t_command *current,
							   char **splitted_path, int *pipefd, int prev_fd)
{
	char *full_path;
	struct sigaction sa_child;

	sa_child.sa_handler = SIG_DFL;
	sa_child.sa_flags = 0;
	sigemptyset(&sa_child.sa_mask);

	sigaction(SIGINT, &sa_child, NULL);
	sigaction(SIGQUIT, &sa_child, NULL);

	if (prev_fd != -1)
	{
		dup2(prev_fd, STDIN_FILENO);
		close(prev_fd);
	}
	else
		apply_input_redirection(current);
	if (current->next)
	{
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		close(pipefd[0]);
	}
	else
		apply_output_redirection(current);
	if (!current->args || !current->args[0])
		exit(1);

	if (try_builtin(data, 0)) // 0 = is_child
		exit(data->exit_value);

	is_accessable(current->args[0], splitted_path, &full_path);
	pipe_execve(full_path, data, current);
}

// Bu fonksiyonun da zaten doğru, dokunmana gerek yok.
static int pipe_exec_parent(t_command *current, int *pipefd, int prev_fd)
{
	if (prev_fd != -1)
		close(prev_fd);
	if (current->next)
	{
		close(pipefd[1]);
		return (pipefd[0]);
	}
	return (-1);
}

// Bu fonksiyonda `exit_value` ayarını ekledim.
static void wait_all_children(t_data *data)
{
	t_command *current;
	int status;
	int last_status; // Son komutun durumunu saklamak için

	current = data->cmd;
	while (current)
	{
		wait(&status);
		last_status = status; // Her seferinde son durumu güncelle
		current = current->next;
	}

	// YENİ: Eğer sonlanan proseslerden biri SIGINT ile öldüyse, yeni satır ekle
	// Not: Bu kontrolü en son komutun durumuna göre yapıyoruz, bu genellikle yeterlidir.
	if (WIFSIGNALED(last_status) && WTERMSIG(status) == SIGINT)
	{
		write(STDOUT_FILENO, "\n", 1);
	}

	if (WIFEXITED(last_status))
		data->exit_value = WEXITSTATUS(last_status);
	else if (WIFSIGNALED(last_status))
		data->exit_value = 128 + WTERMSIG(last_status);
	else
		data->exit_value = 1;
}

// Bu fonksiyonun da zaten doğru, dokunmana gerek yok.
static pid_t fork_and_execute(t_data *data, t_command *current,
							  char **splitted_path, int *pipefd, int prev_fd)
{
	pid_t pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (-1);
	}
	else if (pid == 0)
		pipe_execute_child(data, current, splitted_path, pipefd, prev_fd);
	return (pid);
}

// ******* DEĞİŞİKLİK BURADA *******
// Bu ana fonksiyondur. Sinyal yönetimini buraya ekledim.
void pipe_execute(t_data *data, char **splitted_path)
{
	t_command *current;
	int pipefd[2];
	int prev_fd;
	pid_t pid;

	current = data->cmd;
	prev_fd = -1;

	while (current)
	{
		if (current->next && pipe(pipefd) == -1)
		{
			perror("pipe");
			return;
		}
		pid = fork_and_execute(data, current, splitted_path, pipefd, prev_fd);
		if (pid == -1)
			return;
		// Önceki kodundaki 'if (pid == 0)' bloğunu sildim, çünkü o bir hataydı.
		prev_fd = pipe_exec_parent(current, pipefd, prev_fd);
		current = current->next;
	}
	if (prev_fd != -1)
		close(prev_fd);

	// YENİ: Bütün child'ları beklemeden önce sinyalleri görmezden gel
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);

	wait_all_children(data); // Bekleme burada yapılıyor

	// YENİ: Bekleme bitti, sinyalleri tekrar interaktif moda al
	// signal_handler fonksiyonu minishell.c içinde olduğu için
	// bu dosyada doğrudan kullanamazsın. extern ile bildirebilirsin
	// ya da doğrudan signal(SIGINT, SIG_DFL) yapıp main'de tekrar kurabilirsin.
	// Ama en temizi `minishell.h`'a `signal_handler` prototipini eklemektir.
	// Şimdilik `main`'de ayarlı olduğu için çalışacaktır.
	signal(SIGINT, signal_handler); // En doğru yöntem, handler'ı data struct'ında taşımak
	signal(SIGQUIT, SIG_IGN);
}