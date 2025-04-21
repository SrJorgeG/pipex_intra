/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgomez-d <jgomez-d@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 20:36:04 by jgomez-d          #+#    #+#             */
/*   Updated: 2025/04/21 13:16:07 by jgomez-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	exec_cmd(char *line, char **env)
{
	char	*cmd;
	char	**split;

	split = ft_split(line, ' ');
	if (!split || !*split)
		return (print_error(NULL), ft_free_split(split));
	if (line[0] == '/' || line[0] == '.')
		if (execve(split[0], &split[1], env) == -1)
			return (print_error(*split), ft_free_split(split));
	cmd = get_path(*split, env);
	if (cmd == NULL || execve(cmd, split, env) == -1)
		return (print_error(*split), ft_free_split(split));
	free(cmd);
	return ;
}

void	first_child(int pipe[2], char **av, char **env)
{
	int	factor1;
	int	factor2;
	int	fd;

	factor1 = 1;
	factor2 = 1;
	fd = open(av[1], O_RDONLY);
	if (fd < 0)
	{
		close(pipe[0]);
		close(pipe[1]);
		write(STDERR_FILENO, "Input: infile not valid\n", 25);
		exit(EXIT_FAILURE);
	}
	factor1 *= dup2(fd, STDIN_FILENO);
	factor2 *= dup2(pipe[1], STDOUT_FILENO);
	close(pipe[0]);
	close(pipe[1]);
	if (fd >= 0)
		close(fd);
	if (factor1 < 0 || factor2 < 0)
		exit(EXIT_FAILURE);
	exec_cmd(av[2], env);
}

void	second_child(int pipe[2], char **av, char **env)
{
	int	fd;
	int	factor1;
	int	factor2;

	factor1 = 1;
	factor2 = 1;
	fd = open(av[4], O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd < 0)
	{
		close(pipe[0]);
		close(pipe[1]);
		write(STDERR_FILENO, "Output: outfile not valid\n", 27);
		exit(EXIT_FAILURE);
	}
	factor1 *= dup2(pipe[0], STDIN_FILENO);
	factor2 *= dup2(fd, STDOUT_FILENO);
	close(pipe[0]);
	close(pipe[1]);
	if (fd >= 0)
		close(fd);
	if (factor1 < 0 || factor2 < 0)
		exit(EXIT_FAILURE);
	exec_cmd(av[3], env);
}

int	main(int ac, char *av[], char **env)
{
	int		fd[2];
	pid_t	pid_1;
	pid_t	pid_2;

	if (ac != 5)
		return (write(STDERR_FILENO, "Syntax: ./pipex infile cmd1 cmd2 outfile\n", 41));
	if (pipe(fd) == -1)
		perror("pipe");
	pid_1 = fork();
	if (pid_1 == -1)
		exit(EXIT_FAILURE);
	if (pid_1 == 0)
		first_child(fd, av, env);
	pid_2 = fork();
	if (pid_2 == -1)
		exit(EXIT_FAILURE);
	if (pid_2 == 0)
		second_child(fd, av, env);
	close(fd[0]);
	close(fd[1]);
	waitpid(pid_1, NULL, 0);
	waitpid(pid_2, NULL, 0);
}
