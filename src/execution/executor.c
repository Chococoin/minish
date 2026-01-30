/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: siellage <siellage@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 20:00:00 by glugo-mu          #+#    #+#             */
/*   Updated: 2026/01/30 14:32:30 by siellage         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <sys/wait.h>

static void	exec_child(t_core *core, char *path, t_cmd *cmd, char **envp)
{
	signal(SIGINT, SIG_IGN);
	if (cmd->redirs && apply_redirections(cmd->redirs, envp,
	core->exec_output) < 0)
	{
		free_core(core);
		exit(1);
	}
	setup_child_signals();
	execve(path, cmd->argv, envp);
	perror("minishell");
	free(path);
	free_env(envp);
	free_core(core);
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
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	pid = fork();
	if (pid == -1)
		return (free(path), 1);
	if (pid == 0)
		exec_child(core, path, cmd, envp);
	free(path);
	return (wait_child(pid));
}
