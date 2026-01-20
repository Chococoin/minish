/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glugo-mu <glugo-mu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 20:00:00 by glugo-mu          #+#    #+#             */
/*   Updated: 2026/01/20 11:18:12 by glugo-mu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <sys/wait.h>

static void	exec_child(t_core *core, char *path, t_cmd *cmd, char **envp)
{
	setup_child_signals();
	if (cmd->redirs && apply_redirections(cmd->redirs, envp,
			core->exec_output) < 0)
		exit(1);
	execve(path, cmd->argv, envp);
	perror("minishell");
	exit(127);
}

static int	wait_child(pid_t pid)
{
	int	status;

	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (1);
}

int	execute_external(t_core *core, t_cmd *cmd, char **envp)
{
	pid_t	pid;
	char	*path;

	if (!cmd || !cmd->argv || !cmd->argv[0])
		return (1);
	path = find_in_path(cmd->argv[0], envp);
	if (!path)
	{
		printf("minishell: %s: command not found\n", cmd->argv[0]);
		return (127);
	}
	pid = fork();
	if (pid == -1)
		return (free(path), 1);
	if (pid == 0)
		exec_child(core, path, cmd, envp);
	free(path);
	return (wait_child(pid));
}
