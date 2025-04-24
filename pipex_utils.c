/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgomez-d <jgomez-d@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 06:44:25 by jgomez-d          #+#    #+#             */
/*   Updated: 2025/04/24 04:18:32 by jgomez-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	print_error(char *str)
{
	if (!str)
	{
		write(STDERR_FILENO, " ", 1);
		write(STDERR_FILENO, ": command not found\n", 20);
		exit(EXIT_FAILURE);
	}
	if (errno == 2)
	{
		write(STDERR_FILENO, str, ft_strlen(str));
		write(STDERR_FILENO, ": command not found\n", 20);
		exit(EXIT_FAILURE);
	}
	else
		write(STDERR_FILENO, "cmd: command not found\n", 23);
}

void	ft_free_split(char **split)
{
	int	i;

	i = -1;
	if (!split)
		return ;
	while (split[++i])
		free(split[i]);
	free(split);
}

char	*find(char **env)
{
	int		i;
	int		j;
	char	*sub;

	i = -1;
	while (env[++i])
	{
		j = 0;
		while (env[i][j] && env[i][j] != '=')
			j++;
		sub = ft_substr(env[i], 0, j);
		if (!ft_strncmp(sub, "PATH", 4))
			return (free(sub), env[i] + j + 1);
		free(sub);
	}
	return (NULL);
}

char	*get_path(char *cmd, char **env)
{
	int		i;
	char	**path_env;
	char	*path_part;
	char	*exec;
	char	*path;

	i = 0;
	path = find(env);
	if (!path)
	{
		write(STDERR_FILENO, "path: path not found\n", 22);
		exit(EXIT_FAILURE);
	}
	path_env = ft_split(path, ':');
	if (!path_env)
		return (NULL);
	while (path_env[++i])
	{
		path_part = ft_strjoin(path_env[i], "/");
		exec = ft_strjoin(path_part, cmd);
		free(path_part);
		if (!access(exec, F_OK))
			return (ft_free_split(path_env), exec);
		free(exec);
	}
	return (ft_free_split(path_env), cmd);
}
