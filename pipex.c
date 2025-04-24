/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgomez-d <jgomez-d@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 20:36:04 by jgomez-d          #+#    #+#             */
/*   Updated: 2025/04/24 04:17:25 by jgomez-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	exec_cmd(char *line, char **env)
{
	char	*cmd;
	char	**split;

	while (*line == ' ' || (*line >= 9 && *line <= 13))
		line++;
	split = ft_split(line, ' ');
	if (!split || !*split)
		return (print_error(NULL), ft_free_split(split));
	if (split[0][0] == '/' || split[0][0] == '.')
		if (execve(split[0], &split[0], env) == -1)
			return (print_error(*split), ft_free_split(split));
	cmd = get_path(*split, env);
	if (cmd == NULL || execve(cmd, split, env) == -1)
		return (print_error(*split), ft_free_split(split));
	free(cmd);
}

void	first_child(int pipe[2], char **av, char **env, int fd)
{
	int	factor[2];

	factor[0] = 1;
	factor[1] = 1;
	factor[0] *= dup2(fd, STDIN_FILENO);
	factor[1] *= dup2(pipe[1], STDOUT_FILENO);
	close(pipe[0]);
	close(pipe[1]);
	if (fd >= 0)
		close(fd);
	if (factor[0] < 0 || factor[1] < 0)
		exit(EXIT_FAILURE);
	exec_cmd(av[2], env);
}

void	second_child(int pipe[2], char **av, char **env, int fd)
{
	int	factor[2];

	factor[0] = 1;
	factor[1] = 1;
	factor[0] *= dup2(pipe[0], STDIN_FILENO);
	factor[1] *= dup2(fd, STDOUT_FILENO);
	close(pipe[0]);
	close(pipe[1]);
	if (fd >= 0)
		close(fd);
	if (factor[0] < 0 || factor[1] < 0)
		exit(EXIT_FAILURE);
	exec_cmd(av[3], env);
}

int	*checkfd(char *av[], char **env)
{
	int	*fd;

	if (!env)
	{
		write(STDERR_FILENO, "env: not a correct enviroment\n", 31);
		exit(EXIT_FAILURE);
	}
	fd = malloc(2 * sizeof(int));
	if (!fd)
		exit(EXIT_FAILURE);
	fd[0] = open(av[1], O_RDONLY);
	if (fd[0] < 0)
	{
		write(STDERR_FILENO, "Input: infile not valid\n", 25);
		exit(EXIT_FAILURE);
	}
	fd[1] = open(av[4], O_CREAT | O_WRONLY | O_TRUNC, 0664);
	if (fd[1] < 0)
	{
		write(STDERR_FILENO, "Output: outfile not valid\n", 27);
		exit(EXIT_FAILURE);
	}
	return (fd);
}

int	main(int ac, char *av[], char **env)
{
	int		pipe_fd[2];
	pid_t	pid_1;
	pid_t	pid_2;
	int		*fd;

	if (ac != 5)
		return (write(2, "Syntax: ./pipex infile cmd1 cmd2 outfile\n", 41));
	fd = check(av, env);
	if (pipe(pipe_fd) == -1)
		write(STDERR_FILENO, "pipe: pipe not created\n", 24);
	pid_1 = fork();
	if (pid_1 == -1)
		exit(EXIT_FAILURE);
	if (pid_1 == 0)
		first_child(pipe_fd, av, env, fd[0]);
	pid_2 = fork();
	if (pid_2 == -1)
		exit(EXIT_FAILURE);
	if (pid_2 == 0)
		second_child(pipe_fd, av, env, fd[1]);
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	waitpid(pid_1, NULL, 0);
	waitpid(pid_2, NULL, 0);
}
